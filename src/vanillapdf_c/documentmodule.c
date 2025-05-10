#define PY_SSIZE_T_CLEAN

#include <Python.h>

#define VANILLAPDF_CONFIGURATION_DLL

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_file.h>
#include <vanillapdf/semantics/c_document.h>

PyObject* vp_document_open(PyObject* self, PyObject* args) {
    const char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return NULL;
    }

    DocumentHandle* handle = NULL;
    error_type err = Document_Open(filename, &handle);

    if (err != VANILLAPDF_ERROR_SUCCESS || handle == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to open document");
        return NULL;
    }

    return PyCapsule_New((void*)handle, "VanillaPDF.Document", NULL);
}

PyObject* vp_document_save(PyObject* self, PyObject* args) {
    PyObject* capsule;
    const char* filename;
    if (!PyArg_ParseTuple(args, "Os", &capsule, &filename)) {
        return NULL;
    }

    void* doc = PyCapsule_GetPointer(capsule, "VanillaPDF.Document");
    if (!doc) {
        return NULL;
    }

    int res = Document_Save(doc, filename);
    if (res != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to save document");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject* vp_document_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }

    void* doc = PyCapsule_GetPointer(capsule, "VanillaPDF.Document");
    if (!doc) {
        return NULL;
    }

    Document_Release(doc);
    Py_RETURN_NONE;
}
