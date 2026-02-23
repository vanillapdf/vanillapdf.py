#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "documentmodule.h"
#include "buffermodule.h"
#include "stringobjectmodule.h"
#include "text_string_encodingmodule.h"

static PyMethodDef VanillapdfMethods[] = {
    {"document_open", document_open, METH_VARARGS, "Open a Document"},
    {"document_save", document_save, METH_VARARGS, "Save a Document"},
    {"document_release", document_release, METH_VARARGS, "Release a Document"},

    {"buffer_create", buffer_create, METH_NOARGS, "Create a Buffer"},
    {"buffer_create_from_data", buffer_create_from_data, METH_VARARGS, "Create a Buffer from bytes"},
    {"buffer_set_data", buffer_set_data, METH_VARARGS, "Set Buffer contents"},
    {"buffer_get_data", buffer_get_data, METH_VARARGS, "Get Buffer contents"},
    {"buffer_release", buffer_release, METH_VARARGS, "Release a Buffer"},

    {"literal_string_object_create", literal_string_object_create, METH_VARARGS, "Create a LiteralStringObject from decoded bytes"},
    {"string_object_get_raw", string_object_get_raw, METH_VARARGS, "Get StringObject raw decoded bytes"},
    {"string_object_set_raw", string_object_set_raw, METH_VARARGS, "Set StringObject raw decoded bytes"},
    {"string_object_release", string_object_release, METH_VARARGS, "Release a StringObject"},

    {"text_string_detect_encoding", text_string_detect_encoding, METH_VARARGS, "Detect encoding of PDF text string bytes"},
    {"text_string_pdfdoc_to_unicode", text_string_pdfdoc_to_unicode, METH_VARARGS, "Map a PDFDocEncoding byte to its Unicode code point"},

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
