#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/semantics/c_rectangle.h>

#include "semantics/rectangle.h"
#include "common.h"

static const char* const RECTANGLE_CAPSULE = "VanillaPDF.Rectangle";

PyObject* rectangle_capsule_from_handle(RectangleHandle* handle) {
    return capsule_new(handle, RECTANGLE_CAPSULE, RELEASE_FN(Rectangle_Release));
}

/* Shared bodies for the four get/set coordinate pairs. */
static PyObject* get_coordinate(
        PyObject* args,
        error_type (CALLING_CONVENTION *getter)(RectangleHandle*, bigint_type*),
        const char* operation) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    RectangleHandle* rectangle = capsule_cast<RectangleHandle>(capsule, RECTANGLE_CAPSULE);
    if (rectangle == nullptr) {
        return nullptr;
    }

    bigint_type value = 0;
    error_type err = getter(rectangle, &value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, operation);
    }

    return to_python(value);
}

static PyObject* set_coordinate(
        PyObject* args,
        error_type (CALLING_CONVENTION *setter)(RectangleHandle*, bigint_type),
        const char* operation) {
    PyObject* capsule;
    long long value;
    if (!PyArg_ParseTuple(args, "OL", &capsule, &value)) {
        return nullptr;
    }

    RectangleHandle* rectangle = capsule_cast<RectangleHandle>(capsule, RECTANGLE_CAPSULE);
    if (rectangle == nullptr) {
        return nullptr;
    }

    error_type err = setter(rectangle, value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, operation);
    }

    Py_RETURN_NONE;
}

PyObject* rectangle_create(PyObject* self, PyObject* args) {
    RectangleHandle* rectangle = nullptr;
    error_type err = Rectangle_Create(&rectangle);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Rectangle_Create");
    }

    return rectangle_capsule_from_handle(rectangle);
}

PyObject* rectangle_get_lower_left_x(PyObject* self, PyObject* args) {
    return get_coordinate(args, Rectangle_GetLowerLeftX, "Rectangle_GetLowerLeftX");
}

PyObject* rectangle_set_lower_left_x(PyObject* self, PyObject* args) {
    return set_coordinate(args, Rectangle_SetLowerLeftX, "Rectangle_SetLowerLeftX");
}

PyObject* rectangle_get_lower_left_y(PyObject* self, PyObject* args) {
    return get_coordinate(args, Rectangle_GetLowerLeftY, "Rectangle_GetLowerLeftY");
}

PyObject* rectangle_set_lower_left_y(PyObject* self, PyObject* args) {
    return set_coordinate(args, Rectangle_SetLowerLeftY, "Rectangle_SetLowerLeftY");
}

PyObject* rectangle_get_upper_right_x(PyObject* self, PyObject* args) {
    return get_coordinate(args, Rectangle_GetUpperRightX, "Rectangle_GetUpperRightX");
}

PyObject* rectangle_set_upper_right_x(PyObject* self, PyObject* args) {
    return set_coordinate(args, Rectangle_SetUpperRightX, "Rectangle_SetUpperRightX");
}

PyObject* rectangle_get_upper_right_y(PyObject* self, PyObject* args) {
    return get_coordinate(args, Rectangle_GetUpperRightY, "Rectangle_GetUpperRightY");
}

PyObject* rectangle_set_upper_right_y(PyObject* self, PyObject* args) {
    return set_coordinate(args, Rectangle_SetUpperRightY, "Rectangle_SetUpperRightY");
}

PyObject* rectangle_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, RECTANGLE_CAPSULE);
}
