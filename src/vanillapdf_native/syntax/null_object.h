#ifndef VANILLAPDF_PY_SYNTAX_NULL_OBJECT_H
#define VANILLAPDF_PY_SYNTAX_NULL_OBJECT_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* null_object_create() -> Object */
PyObject* null_object_create(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SYNTAX_NULL_OBJECT_H */
