#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_buffer.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/syntax/c_name_object.h>

#include "syntax/name_object.h"
#include "syntax/object.h"
#include "utils/buffer.h"
#include "common.h"

PyObject* name_object_create_from_decoded_string(PyObject* self, PyObject* args) {
    const char* value;
    if (!PyArg_ParseTuple(args, "s", &value)) {
        return nullptr;
    }

    NameObjectHandle* name = nullptr;
    error_type create_err = NameObject_CreateFromDecodedString(value, &name);
    if (create_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(create_err, "NameObject_CreateFromDecodedString");
    }
    SCOPE_GUARD([name] { NameObject_Release(name); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = NameObject_ToObject(name, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "NameObject_ToObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* name_object_get_value(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    NameObjectHandle* name = object_cast<NameObjectHandle>(
        capsule, NameObject_FromObject, "NameObject_FromObject");
    if (name == nullptr) {
        return nullptr;
    }
    SCOPE_GUARD([name] { NameObject_Release(name); });

    BufferHandle* buffer = nullptr;
    error_type err = NameObject_GetValue(name, &buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "NameObject_GetValue");
    }

    return buffer_capsule_from_handle(buffer);
}
