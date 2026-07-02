#ifndef VANILLAPDF_PY_SYNTAX_INDIRECT_REFERENCE_OBJECT_H
#define VANILLAPDF_PY_SYNTAX_INDIRECT_REFERENCE_OBJECT_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* Indirect reference object. All functions take/return "VanillaPDF.Object". */
PyObject* indirect_reference_object_create(PyObject* self, PyObject* args);
PyObject* indirect_reference_object_get_referenced_object_number(PyObject* self, PyObject* args);
PyObject* indirect_reference_object_get_referenced_generation_number(PyObject* self, PyObject* args);
PyObject* indirect_reference_object_get_referenced_object(PyObject* self, PyObject* args);
PyObject* indirect_reference_object_set_object_number(PyObject* self, PyObject* args);
PyObject* indirect_reference_object_set_generation_number(PyObject* self, PyObject* args);
PyObject* indirect_reference_object_set_referenced_object(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SYNTAX_INDIRECT_REFERENCE_OBJECT_H */
