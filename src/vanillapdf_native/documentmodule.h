#ifndef DOCUMENTMODULE_H
#define DOCUMENTMODULE_H

#include <Python.h>

PyObject* document_open(PyObject* self, PyObject* args);
PyObject* document_save(PyObject* self, PyObject* args);
PyObject* document_release(PyObject* self, PyObject* args);

#endif // DOCUMENTMODULE_H
