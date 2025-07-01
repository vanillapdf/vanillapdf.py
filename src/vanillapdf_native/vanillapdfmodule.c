#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "documentmodule.h"
#include "buffermodule.h"

static PyMethodDef VanillapdfMethods[] = {
    {"document_open", vp_document_open, METH_VARARGS, "Open a Document"},
    {"document_save",   vp_document_save,   METH_VARARGS, "Save a Document"},
    {"document_release", vp_document_release, METH_VARARGS, "Release a Document"},
    {"create", vp_buffer_create, METH_NOARGS, "Create a Buffer"},
    {"create_from_data", vp_buffer_create_from_data, METH_VARARGS, "Create a Buffer from bytes"},
    {"set_data", vp_buffer_set_data, METH_VARARGS, "Set Buffer contents"},
    {"get_data", vp_buffer_get_data, METH_VARARGS, "Get Buffer contents"},
    {"release", vp_buffer_release, METH_VARARGS, "Release a Buffer"},
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
