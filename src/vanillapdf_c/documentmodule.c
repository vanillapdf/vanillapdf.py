#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "vanillapdf/c_document.h"

PyObject* vp_document_create(PyObject* self, PyObject* args) {
    void* doc = Document_Create();
    if (!doc) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create document");
        return NULL;
    }
    return PyCapsule_New(doc, "VanillaPDF.Document", NULL);
}

PyObject* vp_document_save(PyObject* self, PyObject* args) {
    PyObject* capsule;
    const char* filename;
    if (!PyArg_ParseTuple(args, "Os", &capsule, &filename))
        return NULL;
    void* doc = PyCapsule_GetPointer(capsule, "VanillaPDF.Document");
    if (!doc) return NULL;
    int res = Document_Save(doc, filename);
    if (res != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to save document");
        return NULL;
    }
    Py_RETURN_NONE;
}

PyObject* vp_document_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule))
        return NULL;
    void* doc = PyCapsule_GetPointer(capsule, "VanillaPDF.Document");
    if (!doc) return NULL;
    Document_Release(doc);
    Py_RETURN_NONE;
}
