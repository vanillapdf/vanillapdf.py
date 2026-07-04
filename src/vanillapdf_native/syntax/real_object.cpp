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
    error_type create_err = RealObject_CreateFromData(value, precision, &real);
    if (create_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(create_err, "RealObject_CreateFromData");
    }
    SCOPE_GUARD([real] { RealObject_Release(real); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = RealObject_ToObject(real, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "RealObject_ToObject");
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
    SCOPE_GUARD([real] { RealObject_Release(real); });

    real_type value = 0.0;
    error_type err = RealObject_GetValue(real, &value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "RealObject_GetValue");
    }

    return to_python(value);
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
    SCOPE_GUARD([real] { RealObject_Release(real); });

    error_type err = RealObject_SetValue(real, value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "RealObject_SetValue");
    }

    Py_RETURN_NONE;
}
