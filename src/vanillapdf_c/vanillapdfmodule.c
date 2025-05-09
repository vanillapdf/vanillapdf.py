#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "documentmodule.h"

static PyMethodDef VanillapdfMethods[] = {
    {"document_create", vp_document_create, METH_NOARGS, "Create a Document"},
    {"document_save",   vp_document_save,   METH_VARARGS, "Save a Document"},
    {"document_release",vp_document_release,METH_VARARGS, "Release a Document"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef vanillapdfmodule = {
    PyModuleDef_HEAD_INIT,
    "_vanillapdf",
    "VanillaPDF Python bindings",
    -1,
    VanillapdfMethods
};

PyMODINIT_FUNC
PyInit__vanillapdf(void) {
    return PyModule_Create(&vanillapdfmodule);
}
