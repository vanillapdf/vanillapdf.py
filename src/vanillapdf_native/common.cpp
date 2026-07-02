#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_errors.h>

#include "common.h"

/* Definition backing the `extern PyObject* PdfError;` declaration in common.h.
 * nullptr until register_exceptions() creates the exception type at module init;
 * every module includes common.h and shares this one global. */
PyObject* PdfError = nullptr;

/* ---- capsule helpers ---------------------------------------------------- */

/* Shared destructor for every capsule created by capsule_new. Releases the
 * underlying handle (if not already released) and frees the box. */
static void capsule_destructor(PyObject* capsule) {
    /* The capsule name was set by capsule_new; PyCapsule_GetPointer needs it,
     * but the destructor does not know it here. We stored the box as the
     * capsule pointer and the name as the capsule's name, so fetch by name. */
    const char* name = PyCapsule_GetName(capsule);
    HandleBox* box = static_cast<HandleBox*>(PyCapsule_GetPointer(capsule, name));
    if (box != nullptr) {
        if (box->handle != nullptr && box->release != nullptr) {
            box->release(box->handle);
        }
        PyMem_Free(box);
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

    auto handle_guard = make_scope_guard([&] {
        if (release != nullptr) {
            release(handle);
        }
    });

    HandleBox* box = py_malloc<HandleBox>();
    if (box == nullptr) {
        return PyErr_NoMemory();
    }
    auto box_guard = make_scope_guard([&] { PyMem_Free(box); });

    box->handle = handle;
    box->release = release;

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

void* capsule_get(PyObject* capsule, const char* name) {
    if (!PyCapsule_IsValid(capsule, name)) {
        PyErr_Format(PyExc_TypeError, "Invalid %s handle", name);
        return nullptr;
    }

    HandleBox* box = static_cast<HandleBox*>(PyCapsule_GetPointer(capsule, name));
    if (box == nullptr) {
        return nullptr;
    }

    if (box->handle == nullptr) {
        PyErr_Format(PyExc_ValueError, "%s has been released", name);
        return nullptr;
    }

    return box->handle;
}

PyObject* capsule_release(PyObject* capsule, const char* name) {
    if (!PyCapsule_IsValid(capsule, name)) {
        PyErr_Format(PyExc_TypeError, "Invalid %s handle", name);
        return nullptr;
    }

    HandleBox* box = static_cast<HandleBox*>(PyCapsule_GetPointer(capsule, name));
    if (box == nullptr) {
        return nullptr;
    }

    if (box->handle == nullptr) {
        /* Already released - idempotent success. */
        Py_RETURN_NONE;
    }

    if (box->release != nullptr) {
        box->release(box->handle);
    }
    box->handle = nullptr;

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
    error_type length_err = Errors_GetPrintableErrorTextLength(value, &size);
    if (length_err != VANILLAPDF_ERROR_SUCCESS || size == 0) {
        return nullptr;
    }

    char* data = py_malloc<char>(size);
    if (data == nullptr) {
        return nullptr;
    }
    auto data_guard = make_scope_guard([&] { PyMem_Free(data); });

    /* Length already includes the trailing NUL character. */
    error_type text_err = Errors_GetPrintableErrorText(value, data, size);
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
    error_type length_err = Errors_GetLastErrorMessageLength(&size);
    if (length_err != VANILLAPDF_ERROR_SUCCESS || size <= 1) {
        /* size <= 1 means only the trailing NUL: no message. */
        return nullptr;
    }

    char* data = py_malloc<char>(size);
    if (data == nullptr) {
        return nullptr;
    }
    auto data_guard = make_scope_guard([&] { PyMem_Free(data); });

    error_type message_err = Errors_GetLastErrorMessage(data, size);
    if (message_err != VANILLAPDF_ERROR_SUCCESS) {
        return nullptr;
    }

    data_guard.dismiss();  /* ownership passes to the caller */
    return data;
}

PyObject* raise_last_error(error_type err, const char* operation) {
    /* Callers invoke this immediately after a failing library call, on the
     * same thread and with the GIL held, so the thread-local error state has
     * not been disturbed. Note 'err' is the code the failing call returned - we
     * do NOT re-read it from Errors_GetLastError - and the printable text is a
     * pure function of that code. Only 'detail' comes from thread-local storage,
     * and only a same-thread later error could overwrite it (which cannot
     * happen in this inline window). */
    char* printable = fetch_printable_error_text(err);
    char* detail = fetch_last_error_message();

    PyObject* message;
    if (detail != nullptr) {
        message = PyUnicode_FromFormat("%s: %s (%s)",
                                       operation,
                                       printable ? printable : "unknown error",
                                       detail);
    } else {
        message = PyUnicode_FromFormat("%s: %s",
                                       operation,
                                       printable ? printable : "unknown error");
    }

    if (printable != nullptr) {
        PyMem_Free(printable);
    }
    if (detail != nullptr) {
        PyMem_Free(detail);
    }

    if (message == nullptr) {
        return nullptr;
    }

    /* Instantiate PdfError(message) explicitly (rather than PyErr_SetString)
     * so we can attach the numeric error_code attribute to the instance. */
    PyObject* exc = nullptr;
    if (PdfError != nullptr) {
        exc = PyObject_CallFunctionObjArgs(PdfError, message, nullptr);
    }

    if (exc != nullptr) {
        PyObject* code = to_python(err);
        if (code != nullptr) {
            PyObject_SetAttrString(exc, "error_code", code);
            Py_DECREF(code);
        }
        PyErr_SetObject(PdfError, exc);
        Py_DECREF(exc);
    } else {
        /* Fallback if PdfError missing or instantiation failed. */
        PyErr_SetObject(PdfError ? PdfError : PyExc_RuntimeError, message);
    }

    Py_DECREF(message);
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

    /* PyModule_AddObject steals a reference on success but NOT on failure, so
     * we INCREF first to keep our own long-lived global reference. On failure
     * we drop that extra ref and clear the global so it stays nullptr. */
    Py_INCREF(PdfError);
    if (PyModule_AddObject(module, "PdfError", PdfError) < 0) {
        Py_DECREF(PdfError);
        Py_CLEAR(PdfError);
        return -1;
    }

    return 0;
}
