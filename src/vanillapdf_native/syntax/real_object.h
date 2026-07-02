#ifndef VANILLAPDF_PY_SYNTAX_REAL_OBJECT_H
#define VANILLAPDF_PY_SYNTAX_REAL_OBJECT_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* real_object_create(value: float, precision: int) -> Object */
PyObject* real_object_create(PyObject* self, PyObject* args);
/* real_object_get_value(Object) -> float */
PyObject* real_object_get_value(PyObject* self, PyObject* args);
/* real_object_set_value(Object, value: float) -> None */
PyObject* real_object_set_value(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SYNTAX_REAL_OBJECT_H */
