#ifndef VANILLAPDF_PY_SEMANTICS_RECTANGLE_H
#define VANILLAPDF_PY_SEMANTICS_RECTANGLE_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/semantics/c_rectangle.h>

/* Wrap a RectangleHandle in a capsule (takes ownership). */
PyObject* rectangle_capsule_from_handle(RectangleHandle* handle);

/* Rectangle ("VanillaPDF.Rectangle" capsule). Coordinates are integers. */
PyObject* rectangle_create(PyObject* self, PyObject* args);
PyObject* rectangle_get_lower_left_x(PyObject* self, PyObject* args);
PyObject* rectangle_set_lower_left_x(PyObject* self, PyObject* args);
PyObject* rectangle_get_lower_left_y(PyObject* self, PyObject* args);
PyObject* rectangle_set_lower_left_y(PyObject* self, PyObject* args);
PyObject* rectangle_get_upper_right_x(PyObject* self, PyObject* args);
PyObject* rectangle_set_upper_right_x(PyObject* self, PyObject* args);
PyObject* rectangle_get_upper_right_y(PyObject* self, PyObject* args);
PyObject* rectangle_set_upper_right_y(PyObject* self, PyObject* args);
PyObject* rectangle_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SEMANTICS_RECTANGLE_H */
