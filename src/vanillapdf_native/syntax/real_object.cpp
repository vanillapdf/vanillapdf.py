#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/syntax/c_real_object.h>

#include "syntax/real_object.h"
#include "syntax/object.h"
#include "common.h"

PyObject* real_object_create(PyObject* self, PyObject* args) {
    double value;
    int precision;
    if (!PyArg_ParseTuple(args, "di", &value, &precision)) {
        return nullptr;
    }

    RealObjectHandle* real = nullptr;
    error_type err = RealObject_CreateFromData((real_type)value, (integer_type)precision, &real);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "RealObject_CreateFromData");
    }
    auto guard = make_scope_guard([&] { RealObject_Release(real); });

    ObjectHandle* object = nullptr;
    err = RealObject_ToObject(real, &object);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "RealObject_ToObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* real_object_get_value(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    RealObjectHandle* real = object_cast<RealObjectHandle>(
        capsule, RealObject_FromObject, "RealObject_FromObject");
    if (real == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { RealObject_Release(real); });

    real_type value = 0.0;
    error_type err = RealObject_GetValue(real, &value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "RealObject_GetValue");
    }

    return PyFloat_FromDouble((double)value);
}

PyObject* real_object_set_value(PyObject* self, PyObject* args) {
    PyObject* capsule;
    double value;
    if (!PyArg_ParseTuple(args, "Od", &capsule, &value)) {
        return nullptr;
    }

    RealObjectHandle* real = object_cast<RealObjectHandle>(
        capsule, RealObject_FromObject, "RealObject_FromObject");
    if (real == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { RealObject_Release(real); });

    error_type err = RealObject_SetValue(real, (real_type)value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "RealObject_SetValue");
    }

    Py_RETURN_NONE;
}
