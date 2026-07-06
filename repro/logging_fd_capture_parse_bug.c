/*
 * Pure C reproducer (no Python) for a native Vanilla.PDF logging/parsing bug.
 *
 * Mirrors repro/logging_fd_capture_parse_bug.py using the C API directly so the
 * bug can be analyzed in the native library with gdb / AddressSanitizer /
 * ThreadSanitizer, without building the Python bindings.
 *
 * SYMPTOM
 *   Intermittent, spurious VANILLAPDF_ERROR_PARSE_EXCEPTION while parsing a
 *   well-formed PDF.
 *
 * CONDITIONS (all required; remove any one and it never fails)
 *   1. Logging enabled (Logging_SetSeverity(Trace)). The library logs to
 *      STDOUT (fd 1).
 *   2. fd 1 is redirected via dup2() onto a regular file.
 *   3. That file is periodically ftruncate(0)+lseek(0)'d underneath the library
 *      while it keeps writing log lines to it (the write offset is shared via
 *      the dup2'd description). This is what pytest's default --capture=fd does
 *      to its capture buffer between tests, which is how it first surfaced.
 *
 * EXPECTED
 *   logging on  -> "N/300 spurious parse failures" with N > 0
 *   logging off (--off) -> "0/300"
 *
 * BUILD (Linux, adjust include/lib paths to your build tree)
 *   gcc -g -fsanitize=address repro/logging_fd_capture_parse_bug.c \
 *       -I <vanillapdf>/include -L <build>/lib -lvanillapdf \
 *       -o repro && ./repro
 *   (or drop it into the library's C test project and link vanillapdf).
 *
 * MECHANISM
 *   The default stdout log sink's cached OS output handle goes stale/aliased
 *   under the fd juggling, so log writes intermittently land in the wrong file:
 *   the PDF the library is SAVING. The library's own log text (e.g.
 *   "[info] File destructor ...") is written into the saved PDF, clobbering the
 *   "startxref ... %%EOF" tail of the trailer. The file on disk is then
 *   genuinely corrupt, and the parser correctly rejects it. NOT a parser race
 *   or shared parser I/O -- re-parsing the corrupt file keeps failing, and
 *   routing logs through a callback sink (or freopen with a private fd) makes
 *   it vanish. Recommended fix: don't default diagnostics to the stdout sink.
 */
#include <stdio.h>
#include <string.h>

#include <vanillapdf/c_vanillapdf_api.h>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#define DUP _dup
#define DUP2 _dup2
#define FTRUNCATE0(fd) _chsize(fd, 0)
#define LSEEK0(fd) _lseek(fd, 0, SEEK_SET)
#define OPEN_RW(path) _open(path, _O_RDWR | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE)
#define CLOSE _close
#define READ _read
#else
#include <unistd.h>
#include <fcntl.h>
#define DUP dup
#define DUP2 dup2
#define FTRUNCATE0(fd) ftruncate(fd, 0)
#define LSEEK0(fd) lseek(fd, 0, SEEK_SET)
#define OPEN_RW(path) open(path, O_RDWR | O_CREAT | O_TRUNC, 0644)
#define CLOSE close
#define READ read
#endif

#define STDOUT_FD 1
#define ITERATIONS 300

static const char* PDF_PATH = "vpdf_repro_doc.pdf";
static const char* CAP_PATH = "vpdf_repro_capture.log";

#define CHECK(expr)                                        \
    do {                                                   \
        error_type _rc = (expr);                           \
        if (_rc != VANILLAPDF_ERROR_SUCCESS) { return _rc; } \
    } while (0)

/* Create a minimal one-page PDF. */
static error_type make_pdf(const char* path) {
    DocumentHandle* doc = NULL;
    PageObjectHandle* page = NULL;
    RectangleHandle* rect = NULL;
    CatalogHandle* catalog = NULL;
    PageTreeHandle* pages = NULL;

    CHECK(Document_Create(path, &doc));
    CHECK(PageObject_CreateFromDocument(doc, &page));
    CHECK(Rectangle_Create(&rect));
    CHECK(Rectangle_SetLowerLeftX(rect, 0));
    CHECK(Rectangle_SetLowerLeftY(rect, 0));
    CHECK(Rectangle_SetUpperRightX(rect, 200));
    CHECK(Rectangle_SetUpperRightY(rect, 300));
    CHECK(PageObject_SetMediaBox(page, rect));
    CHECK(Document_GetCatalog(doc, &catalog));
    CHECK(Catalog_GetPages(catalog, &pages));
    CHECK(PageTree_AppendPage(pages, page));
    CHECK(Document_Save(doc, path));

    PageTree_Release(pages);
    Catalog_Release(catalog);
    Rectangle_Release(rect);
    PageObject_Release(page);
    Document_Release(doc);
    return VANILLAPDF_ERROR_SUCCESS;
}

/* Open + initialize + parse every object (plenty of log output). Returns the
 * error code of the first failing parse, or SUCCESS. */
static error_type deep_parse(const char* path) {
    FileHandle* file = NULL;
    error_type init_rc;

    CHECK(File_Open(path, &file));
    init_rc = File_Initialize(file);
    if (init_rc != VANILLAPDF_ERROR_SUCCESS) {
        File_Release(file);
        return init_rc;
    }

    for (biguint_type num = 1; num <= 5; ++num) {
        ObjectHandle* obj = NULL;
        error_type get_rc = File_GetIndirectObject(file, num, 0, &obj);
        if (get_rc != VANILLAPDF_ERROR_SUCCESS) {
            File_Release(file);
            return get_rc;
        }
        BufferHandle* buffer = NULL;
        if (Object_ToPdf(obj, &buffer) == VANILLAPDF_ERROR_SUCCESS) {
            Buffer_Release(buffer);
        }
        Object_Release(obj);
    }

    File_Release(file);
    return VANILLAPDF_ERROR_SUCCESS;
}

int main(int argc, char** argv) {
    int logging_off = (argc > 1 && strcmp(argv[1], "--off") == 0);
    char scratch[1 << 16];

    Logging_SetSeverity(logging_off ? LoggingSeverity_Off : LoggingSeverity_Trace);

    int saved_stdout = DUP(STDOUT_FD);
    int cap_fd = OPEN_RW(CAP_PATH);
    DUP2(cap_fd, STDOUT_FD); /* fd 1 -> capture file, mimics pytest --capture=fd */

    int fails = 0;
    for (int i = 0; i < ITERATIONS; ++i) {
        error_type make_rc = make_pdf(PDF_PATH);
        error_type parse_rc = (make_rc == VANILLAPDF_ERROR_SUCCESS)
                                  ? deep_parse(PDF_PATH)
                                  : make_rc;
        if (parse_rc != VANILLAPDF_ERROR_SUCCESS) {
            ++fails;
            if (fails <= 3) {
                DUP2(saved_stdout, STDOUT_FD);
                fprintf(stderr, "iter %d: error_type=%u\n", i, (unsigned)parse_rc);
                DUP2(cap_fd, STDOUT_FD);
            }
        }
        /* mimic pytest reading + truncating the capture buffer between tests */
        LSEEK0(cap_fd);
        (void)READ(cap_fd, scratch, sizeof(scratch));
        LSEEK0(cap_fd);
        FTRUNCATE0(cap_fd);
    }

    DUP2(saved_stdout, STDOUT_FD);
    CLOSE(saved_stdout);
    CLOSE(cap_fd);

    fprintf(stderr, "logging=%s: %d/%d spurious parse failures\n",
            logging_off ? "OFF" : "TRACE", fails, ITERATIONS);
    return (fails && logging_off) ? 1 : 0;
}
