#ifndef VANILLAPDF_PY_SYNTAX_NAME_OBJECT_H
#define VANILLAPDF_PY_SYNTAX_NAME_OBJECT_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* name_object_create_from_decoded_string(value: str) -> Object */
PyObject* name_object_create_from_decoded_string(PyObject* self, PyObject* args);
/* name_object_get_value(Object) -> Buffer */
PyObject* name_object_get_value(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SYNTAX_NAME_OBJECT_H */
