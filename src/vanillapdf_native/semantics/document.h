#ifndef VANILLAPDF_PY_SEMANTICS_DOCUMENT_H
#define VANILLAPDF_PY_SEMANTICS_DOCUMENT_H

#include <Python.h>

PyObject* document_open(PyObject* self, PyObject* args);
PyObject* document_save(PyObject* self, PyObject* args);
PyObject* document_release(PyObject* self, PyObject* args);

#endif // VANILLAPDF_PY_SEMANTICS_DOCUMENT_H
