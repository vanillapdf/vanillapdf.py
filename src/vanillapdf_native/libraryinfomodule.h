#ifndef LIBRARYINFOMODULE_H
#define LIBRARYINFOMODULE_H

#include <Python.h>

PyObject* library_info_get_version_major(PyObject* self, PyObject* args);
PyObject* library_info_get_version_minor(PyObject* self, PyObject* args);
PyObject* library_info_get_version_patch(PyObject* self, PyObject* args);
PyObject* library_info_get_version_build(PyObject* self, PyObject* args);
PyObject* library_info_get_author(PyObject* self, PyObject* args);
PyObject* library_info_get_build_day(PyObject* self, PyObject* args);
PyObject* library_info_get_build_month(PyObject* self, PyObject* args);
PyObject* library_info_get_build_year(PyObject* self, PyObject* args);

#endif // LIBRARYINFOMODULE_H
