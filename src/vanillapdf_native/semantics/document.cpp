#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/semantics/c_document.h>

#include "semantics/document.h"
#include "common.h"

static const char* const DOCUMENT_CAPSULE = "VanillaPDF.Document";

PyObject* document_open(PyObject* self, PyObject* args) {
    const char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return nullptr;
    }

    DocumentHandle* handle = nullptr;
    error_type err = Document_Open(filename, &handle);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_Open");
    }

    return capsule_new(handle, DOCUMENT_CAPSULE, RELEASE_FN(Document_Release));
}

PyObject* document_save(PyObject* self, PyObject* args) {
    PyObject* capsule;
    const char* filename;
    if (!PyArg_ParseTuple(args, "Os", &capsule, &filename)) {
        return nullptr;
    }

    DocumentHandle* handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    error_type err = Document_Save(handle, filename);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_Save");
    }

    Py_RETURN_NONE;
}

PyObject* document_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, DOCUMENT_CAPSULE);
}
