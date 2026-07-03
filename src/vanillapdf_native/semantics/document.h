#ifndef VANILLAPDF_PY_SEMANTICS_DOCUMENT_H
#define VANILLAPDF_PY_SEMANTICS_DOCUMENT_H

#include <Python.h>

PyObject* document_open(PyObject* self, PyObject* args);
PyObject* document_create(PyObject* self, PyObject* args);
PyObject* document_open_file(PyObject* self, PyObject* args);
PyObject* document_save(PyObject* self, PyObject* args);
PyObject* document_save_incremental(PyObject* self, PyObject* args);
PyObject* document_get_catalog(PyObject* self, PyObject* args);
PyObject* document_get_document_info(PyObject* self, PyObject* args);
PyObject* document_append_document(PyObject* self, PyObject* args);
PyObject* document_add_encryption(PyObject* self, PyObject* args);
PyObject* document_remove_encryption(PyObject* self, PyObject* args);
PyObject* document_release(PyObject* self, PyObject* args);

#endif // VANILLAPDF_PY_SEMANTICS_DOCUMENT_H
