#ifndef DOCUMENTMODULE_H
#define DOCUMENTMODULE_H

#include <Python.h>

PyObject* vp_document_create(PyObject* self, PyObject* args);
PyObject* vp_document_save(PyObject* self, PyObject* args);
PyObject* vp_document_release(PyObject* self, PyObject* args);

#endif // DOCUMENTMODULE_H
