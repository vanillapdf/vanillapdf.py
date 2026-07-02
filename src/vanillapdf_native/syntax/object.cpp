#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_object.h>

#include "syntax/object.h"
#include "utils/buffer.h"
#include "common.h"

const char* const OBJECT_CAPSULE = "VanillaPDF.Object";

PyObject* object_capsule_from_handle(ObjectHandle* handle) {
    return capsule_new(handle, OBJECT_CAPSULE, RELEASE_FN(Object_Release));
}

PyObject* object_get_object_type(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    ObjectHandle* object = capsule_cast<ObjectHandle>(capsule, OBJECT_CAPSULE);
    if (object == nullptr) {
        return nullptr;
    }

    ObjectType type = ObjectType_Undefined;
    error_type err = Object_GetObjectType(object, &type);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Object_GetObjectType");
    }

    return PyLong_FromLong(static_cast<long>(type));
}

PyObject* object_type_name(PyObject* self, PyObject* args) {
    int type_value;
    if (!PyArg_ParseTuple(args, "i", &type_value)) {
        return nullptr;
    }

    string_type name = nullptr;
    error_type err = Object_TypeName(static_cast<ObjectType>(type_value), &name);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Object_TypeName");
    }

    return PyUnicode_FromString(name);
}

PyObject* object_get_offset(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    ObjectHandle* object = capsule_cast<ObjectHandle>(capsule, OBJECT_CAPSULE);
    if (object == nullptr) {
        return nullptr;
    }

    offset_type offset = 0;
    error_type err = Object_GetOffset(object, &offset);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Object_GetOffset");
    }

    return PyLong_FromLongLong(static_cast<long long>(offset));
}

PyObject* object_to_string(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    ObjectHandle* object = capsule_cast<ObjectHandle>(capsule, OBJECT_CAPSULE);
    if (object == nullptr) {
        return nullptr;
    }

    BufferHandle* buffer = nullptr;
    error_type err = Object_ToString(object, &buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Object_ToString");
    }

    return buffer_capsule_from_handle(buffer);
}

PyObject* object_to_pdf(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    ObjectHandle* object = capsule_cast<ObjectHandle>(capsule, OBJECT_CAPSULE);
    if (object == nullptr) {
        return nullptr;
    }

    BufferHandle* buffer = nullptr;
    error_type err = Object_ToPdf(object, &buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Object_ToPdf");
    }

    return buffer_capsule_from_handle(buffer);
}

PyObject* object_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, OBJECT_CAPSULE);
}
