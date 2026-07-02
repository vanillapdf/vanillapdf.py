#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/syntax/c_integer_object.h>

#include "syntax/integer_object.h"
#include "syntax/object.h"
#include "common.h"

PyObject* integer_object_create(PyObject* self, PyObject* args) {
    long long value;
    if (!PyArg_ParseTuple(args, "L", &value)) {
        return nullptr;
    }

    IntegerObjectHandle* integer = nullptr;
    error_type create_err = IntegerObject_CreateFromIntegerValue(static_cast<bigint_type>(value), &integer);
    if (create_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(create_err, "IntegerObject_CreateFromIntegerValue");
    }
    auto guard = make_scope_guard([&] { IntegerObject_Release(integer); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = IntegerObject_ToObject(integer, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "IntegerObject_ToObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* integer_object_get_value(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    IntegerObjectHandle* integer = object_cast<IntegerObjectHandle>(
        capsule, IntegerObject_FromObject, "IntegerObject_FromObject");
    if (integer == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { IntegerObject_Release(integer); });

    bigint_type value = 0;
    error_type err = IntegerObject_GetIntegerValue(integer, &value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "IntegerObject_GetIntegerValue");
    }

    return PyLong_FromLongLong(static_cast<long long>(value));
}

PyObject* integer_object_set_value(PyObject* self, PyObject* args) {
    PyObject* capsule;
    long long value;
    if (!PyArg_ParseTuple(args, "OL", &capsule, &value)) {
        return nullptr;
    }

    IntegerObjectHandle* integer = object_cast<IntegerObjectHandle>(
        capsule, IntegerObject_FromObject, "IntegerObject_FromObject");
    if (integer == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { IntegerObject_Release(integer); });

    error_type err = IntegerObject_SetIntegerValue(integer, static_cast<bigint_type>(value));
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "IntegerObject_SetIntegerValue");
    }

    Py_RETURN_NONE;
}
