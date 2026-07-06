#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/syntax/c_null_object.h>

#include "syntax/null_object.h"
#include "syntax/object.h"
#include "common.h"

PyObject* null_object_create(PyObject* self, PyObject* args) {
    NullObjectHandle* null_object = nullptr;
    error_type create_err = NullObject_Create(&null_object);
    if (create_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(create_err, "NullObject_Create");
    }
    SCOPE_GUARD([null_object] { NullObject_Release(null_object); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = NullObject_ToObject(null_object, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "NullObject_ToObject");
    }

    return object_capsule_from_handle(object);
}
