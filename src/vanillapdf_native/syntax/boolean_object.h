#ifndef VANILLAPDF_PY_SYNTAX_BOOLEAN_OBJECT_H
#define VANILLAPDF_PY_SYNTAX_BOOLEAN_OBJECT_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* boolean_object_create(value: bool) -> Object */
PyObject* boolean_object_create(PyObject* self, PyObject* args);
/* boolean_object_get_value(Object) -> bool */
PyObject* boolean_object_get_value(PyObject* self, PyObject* args);
/* boolean_object_set_value(Object, value: bool) -> None */
PyObject* boolean_object_set_value(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SYNTAX_BOOLEAN_OBJECT_H */
