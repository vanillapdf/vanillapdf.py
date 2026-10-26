#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <atomic>
#include <cstdint>
#include <new>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_errors.h>

#include "common.h"

/* Definition backing the `extern PyObject* PdfError;` declaration in common.h.
 * nullptr until register_exceptions() creates the exception type at module init;
 * every module includes common.h and shares this one global. */
PyObject* PdfError = nullptr;

/* ---- capsule helpers ---------------------------------------------------- */

/* Heap-allocated box that every capsule points to. It owns one native reference
 * to `handle` and counts the binding calls using it, like a .NET SafeHandle:
 *
 *   state = (references << 1) | CLOSED
 *
 * The capsule holds one reference from creation until close() (or the capsule
 * destructor) sets CLOSED and drops it; each call holds one more while it runs
 * (HandleRef in common.h).
 * Whoever drops the last reference performs the native release, exactly once,
 * and no reference can be taken once CLOSED is set. All transitions are atomic,
 * so this holds without the GIL too (free-threaded builds). */
struct HandleBox {
    void* handle;
    release_fn release;
    std::atomic<uint64_t> state;
};

static const uint64_t CLOSED = 1;
static const uint64_t ONE_REF = 2;

/* Take a reference for a call. False if the box has been closed. */
static bool box_acquire(HandleBox* box) {
    uint64_t state = box->state.load();
    do {
        if (state & CLOSED) {
            return false;
        }
    } while (!box->state.compare_exchange_weak(state, state + ONE_REF));
    return true;
}

/* Drop a reference; the last one (only reachable once CLOSED is set, since the
 * capsule's own reference is dropped only by closing) releases the handle. */
static void box_release_ref(HandleBox* box) {
    if (box->state.fetch_sub(ONE_REF) == (CLOSED | ONE_REF)) {
        if (box->release != nullptr) {
            release_without_gil(box->release, box->handle);
        }
    }
}

/* Set CLOSED and drop the capsule's own reference. Idempotent: only the call
 * that sets CLOSED drops it. */
static void box_close(HandleBox* box) {
    uint64_t state = box->state.load();
    do {
        if (state & CLOSED) {
            return;
        }
    } while (!box->state.compare_exchange_weak(state, state | CLOSED));
    box_release_ref(box);
}

/* Shared destructor for every capsule created by capsule_new. No call can be
 * using the box (a running call's arguments keep the capsule alive), so closing
 * releases the handle here unless close() already did. */
static void capsule_destructor(PyObject* capsule) {
    /* The capsule name was set by capsule_new; PyCapsule_GetPointer needs it,
     * but the destructor does not know it here. We stored the box as the
     * capsule pointer and the name as the capsule's name, so fetch by name. */
    const char* name = PyCapsule_GetName(capsule);
    HandleBox* box = static_cast<HandleBox*>(PyCapsule_GetPointer(capsule, name));
    if (box != nullptr) {
        box_close(box);
        delete box;
    }
}

PyObject* capsule_new(void* handle, const char* name, release_fn release) {
    /* Takes ownership of 'handle': the scope guards below release it (and free
     * the box) on any failure, so the caller can pass the handle and forget
     * about it - callers write `return capsule_new(handle, ...)` with no cleanup. */
    if (handle == nullptr) {
        PyErr_SetString(PyExc_ValueError,
                        "Cannot create capsule from a null handle");
        return nullptr;
    }

    auto handle_guard = make_scope_guard([handle, release] {
        if (release != nullptr) {
            release_without_gil(release, handle);
        }
    });

    /* new, not py_malloc: the atomic state must be constructed. */
    HandleBox* box = new (std::nothrow) HandleBox{handle, release, {ONE_REF}};
    if (box == nullptr) {
        return PyErr_NoMemory();
    }
    auto box_guard = make_scope_guard([box] { delete box; });

    /* PyCapsule_New stores 'name' by pointer (not copied), so callers must
     * pass a string literal with static lifetime. */
    PyObject* capsule = PyCapsule_New(box, name, capsule_destructor);
    if (capsule == nullptr) {
        return nullptr;
    }

    /* The capsule now owns the box, and through it the handle. */
    box_guard.dismiss();
    handle_guard.dismiss();
    return capsule;
}

/* Validate a capsule of the given name and return its box, or nullptr with a
 * Python TypeError set. Shared preamble for capsule_get / capsule_release.
 * PyCapsule_IsValid guarantees a non-NULL stored pointer, so GetPointer here
 * succeeds. */
static HandleBox* get_valid_box(PyObject* capsule, const char* name) {
    if (!PyCapsule_IsValid(capsule, name)) {
        PyErr_Format(PyExc_TypeError, "Invalid %s handle", name);
        return nullptr;
    }

    return static_cast<HandleBox*>(PyCapsule_GetPointer(capsule, name));
}

void* handle_box_acquire(PyObject* capsule, const char* name, HandleBox** box) {
    HandleBox* candidate = get_valid_box(capsule, name);
    if (candidate == nullptr) {
        return nullptr;
    }

    if (!box_acquire(candidate)) {
        PyErr_Format(PyExc_ValueError, "%s has been released", name);
        return nullptr;
    }

    *box = candidate;
    return candidate->handle;
}

void handle_box_release(HandleBox* box) {
    box_release_ref(box);
}

PyObject* capsule_release(PyObject* capsule, const char* name) {
    HandleBox* box = get_valid_box(capsule, name);
    if (box == nullptr) {
        return nullptr;
    }

    box_close(box);
    Py_RETURN_NONE;
}

/* ---- error handling ----------------------------------------------------- */

int is_missing_error(error_type err) {
    return (err == VANILLAPDF_ERROR_OBJECT_MISSING
            || err == VANILLAPDF_ERROR_OPTIONAL_ENTRY_MISSING);
}

/* Fetch the printable name for an error code. Returns a newly PyMem_Malloc'd
 * NUL-terminated string, or nullptr (no exception set) on failure. */
static char* fetch_printable_error_text(error_type value) {
    size_type size = 0;
    error_type length_err = without_gil([value, &size] {
        return Errors_GetPrintableErrorTextLength(value, &size);
    });
    if (length_err != VANILLAPDF_ERROR_SUCCESS || size == 0) {
        return nullptr;
    }

    char* data = py_malloc<char>(size);
    if (data == nullptr) {
        return nullptr;
    }
    auto data_guard = make_scope_guard([data] { PyMem_Free(data); });

    /* Length already includes the trailing NUL character. */
    error_type text_err = without_gil([value, data, size] {
        return Errors_GetPrintableErrorText(value, data, size);
    });
    if (text_err != VANILLAPDF_ERROR_SUCCESS) {
        return nullptr;
    }

    data_guard.dismiss();  /* ownership passes to the caller */
    return data;
}

/* Fetch the thread-local detail message for the last error. Returns a newly
 * PyMem_Malloc'd NUL-terminated string, or nullptr on failure/empty. */
static char* fetch_last_error_message(void) {
    size_type size = 0;
    error_type length_err = without_gil([&size] {
        return Errors_GetLastErrorMessageLength(&size);
    });
    if (length_err != VANILLAPDF_ERROR_SUCCESS || size <= 1) {
        /* size <= 1 means only the trailing NUL: no message. */
        return nullptr;
    }

    char* data = py_malloc<char>(size);
    if (data == nullptr) {
        return nullptr;
    }
    auto data_guard = make_scope_guard([data] { PyMem_Free(data); });

    error_type message_err = without_gil([data, size] {
        return Errors_GetLastErrorMessage(data, size);
    });
    if (message_err != VANILLAPDF_ERROR_SUCCESS) {
        return nullptr;
    }

    data_guard.dismiss();  /* ownership passes to the caller */
    return data;
}

/* Build the "operation: NAME (detail)" message string for an error.
 *
 * 'err' is the code the failing call returned - we do NOT re-read it from
 * Errors_GetLastError, and the printable NAME is a pure function of it. Only the
 * optional 'detail' comes from thread-local storage; callers invoke us inline
 * right after the failing call (same thread, GIL held), so it is still current.
 * Returns a new reference, or nullptr with a Python exception set. */
static PyObject* build_error_message(error_type err, const char* operation) {
    char* printable = fetch_printable_error_text(err);
    SCOPE_GUARD([printable] { PyMem_Free(printable); });

    char* detail = fetch_last_error_message();
    SCOPE_GUARD([detail] { PyMem_Free(detail); });

    const char* name = printable ? printable : "unknown error";
    if (detail) {
        return PyUnicode_FromFormat("%s: %s (%s)", operation, name, detail);
    }
    return PyUnicode_FromFormat("%s: %s", operation, name);
}

/* Create a PdfError instance carrying 'message' and the numeric 'error_code'.
 * Returns a new reference, or nullptr with a Python exception set. */
static PyObject* make_pdf_error(PyObject* message, error_type err) {
    PyObject* exc = PyObject_CallFunctionObjArgs(PdfError, message, nullptr);
    if (exc == nullptr) {
        return nullptr;
    }

    PyObject* code = to_python(err);
    if (code != nullptr) {
        PyObject_SetAttrString(exc, "error_code", code);
        Py_DECREF(code);
    }

    return exc;
}

PyObject* raise_last_error(error_type err, const char* operation) {
    PyObject* message = build_error_message(err, operation);
    if (message == nullptr) {
        return nullptr;
    }
    SCOPE_GUARD([message] { Py_DECREF(message); });

    /* PdfError is created at module init; if that somehow failed, degrade to a
     * plain RuntimeError so we still raise something meaningful. */
    if (PdfError == nullptr) {
        PyErr_SetObject(PyExc_RuntimeError, message);
        return nullptr;
    }

    /* Prefer a PdfError instance with the error_code attribute; if building it
     * fails (e.g. out of memory), still raise PdfError with just the message. */
    PyObject* exc = make_pdf_error(message, err);
    if (exc == nullptr) {
        PyErr_Clear();
        PyErr_SetObject(PdfError, message);
        return nullptr;
    }

    PyErr_SetObject(PdfError, exc);
    Py_DECREF(exc);
    return nullptr;
}

int register_exceptions(PyObject* module) {
    PdfError = PyErr_NewExceptionWithDoc(
        "_vanillapdf.PdfError",
        "Raised when a VanillaPDF library call fails. The numeric library "
        "error code is available in the 'error_code' attribute.",
        nullptr,   /* base: Exception */
        nullptr);  /* dict */
    if (PdfError == nullptr) {
        return -1;
    }

    /* PyModule_AddObjectRef (3.10+) does not steal a reference: it adds its own
     * on success and leaves our global holding the one from NewExceptionWithDoc.
     * On failure, clear the global so it stays nullptr. */
    if (PyModule_AddObjectRef(module, "PdfError", PdfError) < 0) {
        Py_CLEAR(PdfError);
        return -1;
    }

    return 0;
}
