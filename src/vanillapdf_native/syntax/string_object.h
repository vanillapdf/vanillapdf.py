#ifndef VANILLAPDF_PY_SYNTAX_STRING_OBJECT_H
#define VANILLAPDF_PY_SYNTAX_STRING_OBJECT_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* String (base) */
PyObject* string_object_get_string_type(PyObject* self, PyObject* args);
PyObject* string_object_get_value(PyObject* self, PyObject* args);
PyObject* string_object_set_value(PyObject* self, PyObject* args);

/* Literal / Hexadecimal string constructors (return a base String Object) */
PyObject* literal_string_object_create_from_decoded_string(PyObject* self, PyObject* args);
PyObject* hexadecimal_string_object_create_from_decoded_string(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SYNTAX_STRING_OBJECT_H */
