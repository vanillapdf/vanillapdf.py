#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/syntax/c_boolean_object.h>

#include "syntax/boolean_object.h"
#include "syntax/object.h"
#include "common.h"

PyObject* boolean_object_create(PyObject* self, PyObject* args) {
    int value;
    if (!PyArg_ParseTuple(args, "p", &value)) {
        return nullptr;
    }

    BooleanObjectHandle* boolean = nullptr;
    error_type create_err = BooleanObject_Create(&boolean);
    if (create_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(create_err, "BooleanObject_Create");
    }
    SCOPE_GUARD([boolean] { BooleanObject_Release(boolean); });

    error_type set_err = BooleanObject_SetValue(boolean, value ? VANILLAPDF_RV_TRUE : VANILLAPDF_RV_FALSE);
    if (set_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(set_err, "BooleanObject_SetValue");
    }

    ObjectHandle* object = nullptr;
    error_type to_object_err = BooleanObject_ToObject(boolean, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "BooleanObject_ToObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* boolean_object_get_value(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    BooleanObjectHandle* boolean = object_cast<BooleanObjectHandle>(
        capsule, BooleanObject_FromObject, "BooleanObject_FromObject");
    if (boolean == nullptr) {
        return nullptr;
    }
    SCOPE_GUARD([boolean] { BooleanObject_Release(boolean); });

    boolean_type value = VANILLAPDF_RV_FALSE;
    error_type err = BooleanObject_GetValue(boolean, &value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "BooleanObject_GetValue");
    }

    if (value == VANILLAPDF_RV_TRUE) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

PyObject* boolean_object_set_value(PyObject* self, PyObject* args) {
    PyObject* capsule;
    int value;
    if (!PyArg_ParseTuple(args, "Op", &capsule, &value)) {
        return nullptr;
    }

    BooleanObjectHandle* boolean = object_cast<BooleanObjectHandle>(
        capsule, BooleanObject_FromObject, "BooleanObject_FromObject");
    if (boolean == nullptr) {
        return nullptr;
    }
    SCOPE_GUARD([boolean] { BooleanObject_Release(boolean); });

    error_type err = BooleanObject_SetValue(boolean, value ? VANILLAPDF_RV_TRUE : VANILLAPDF_RV_FALSE);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "BooleanObject_SetValue");
    }

    Py_RETURN_NONE;
}
