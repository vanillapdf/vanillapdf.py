#ifndef VANILLAPDF_PY_COMMON_H
#define VANILLAPDF_PY_COMMON_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_export.h>
#include <vanillapdf/c_types.h>

/* Release function signature shared by all VanillaPDF handle types.
 * The actual functions take typed handles; they are ABI-compatible
 * with a single void pointer argument on all supported platforms. */
typedef error_type (CALLING_CONVENTION *release_fn)(void*);

/* Cast helper for typed release functions (e.g. Buffer_Release) */
#define RELEASE_FN(fn) ((release_fn)(fn))

/* Heap-allocated box that every capsule points to.
 * handle == NULL means the underlying object has been released. */
typedef struct {
    void* handle;
    release_fn release;
} HandleBox;

/* Exception type raised for VanillaPDF library errors.
 * Created in register_exceptions(), exposed as _vanillapdf.PdfError
 * and re-exported as vanillapdf.PdfError. Instances carry the numeric
 * library error code in the 'error_code' attribute. */
extern PyObject* PdfError;

/* Register exception types on the module. Returns 0 on success. */
int register_exceptions(PyObject* module);

/* Fetch the error description from the library and raise PdfError.
 * 'operation' names the failed call for context. Always returns NULL
 * so callers can write: return raise_last_error(err, "File_Open"); */
PyObject* raise_last_error(error_type err, const char* operation);

/* Wrap a VanillaPDF handle in a new capsule. Takes ownership of the
 * handle: on failure the handle is released and NULL is returned with
 * an exception set. 'name' must be a string literal (stored, not copied). */
PyObject* capsule_new(void* handle, const char* name, release_fn release);

/* Extract the handle from a capsule created by capsule_new.
 * Returns NULL with an exception set if the capsule is invalid or
 * the handle has already been released. */
void* capsule_get(PyObject* capsule, const char* name);

/* Explicitly release the handle inside a capsule (idempotent).
 * Returns Py_None on success, NULL with an exception on invalid capsule. */
PyObject* capsule_release(PyObject* capsule, const char* name);

/* Returns 1 if the error signals a missing optional entry */
int is_missing_error(error_type err);

#ifdef __cplusplus

#include <utility>

#include "conversion.h"

/* The size_type -> size_t conversions used for allocation are lossless as long
 * as size_t is at least as wide as the library's size_type. All supported
 * targets are 64-bit, so this holds; enforce it at compile time so a narrower
 * platform fails to build rather than truncating a size silently. */
static_assert(sizeof(size_t) >= sizeof(size_type),
              "size_type must fit into size_t");

/* Minimal RAII scope guard: runs `fn` when it goes out of scope, unless
 * dismiss() has been called. Lets error paths bail with a plain `return`
 * instead of repeating handle-release / free cleanup at every early exit.
 * Modeled on vanillapdf's ScopeGuard (its util.h is library-internal, so we
 * keep a small self-contained copy here).
 *
 *   auto guard = make_scope_guard([&] { PyMem_Free(data); });
 *   ...
 *   guard.dismiss();   // ownership transferred; skip the cleanup */
template <typename Fn>
class ScopeGuard {
public:
    explicit ScopeGuard(Fn fn) : _fn(std::move(fn)) {}
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&& other) : _fn(std::move(other._fn)), _active(other._active) {
        other._active = false;
    }
    ~ScopeGuard() { if (_active) _fn(); }

    void dismiss() { _active = false; }

private:
    Fn _fn;
    bool _active = true;
};

template <typename Fn>
ScopeGuard<Fn> make_scope_guard(Fn fn) {
    return ScopeGuard<Fn>(std::move(fn));
}

/* Allocate raw, uninitialized memory for `count` objects of type T via
 * PyMem_Malloc, returning a typed pointer (nullptr on failure, no exception
 * set - callers raise PyErr_NoMemory). Does not construct T; intended for the
 * POD boxes and char buffers this extension manages.
 *
 *   HandleBox* box = py_malloc<HandleBox>();
 *   char* data = py_malloc<char>(size); */
template <typename T>
T* py_malloc(size_t count = 1) {
    return static_cast<T*>(PyMem_Malloc(count * sizeof(T)));
}

/* Typed convenience wrapper over capsule_get: extracts the handle and casts it
 * to T*. Returns nullptr with a Python exception set if the capsule is invalid
 * or the handle has already been released.
 *
 *   auto* buffer = capsule_cast<BufferHandle>(capsule, "VanillaPDF.Buffer"); */
template <typename T>
T* capsule_cast(PyObject* capsule, const char* name) {
    return static_cast<T*>(capsule_get(capsule, name));
}

#endif /* __cplusplus */

#endif /* VANILLAPDF_PY_COMMON_H */
