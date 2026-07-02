#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/syntax/c_array_object.h>

#include "syntax/array_object.h"
#include "syntax/object.h"
#include "common.h"

PyObject* array_object_create(PyObject* self, PyObject* args) {
    ArrayObjectHandle* array = nullptr;
    error_type create_err = ArrayObject_Create(&array);
    if (create_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(create_err, "ArrayObject_Create");
    }
    auto guard = make_scope_guard([&] { ArrayObject_Release(array); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = ArrayObject_ToObject(array, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "ArrayObject_ToObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* array_object_get_size(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    ArrayObjectHandle* array = object_cast<ArrayObjectHandle>(
        capsule, ArrayObject_FromObject, "ArrayObject_FromObject");
    if (array == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { ArrayObject_Release(array); });

    size_type size = 0;
    error_type err = ArrayObject_GetSize(array, &size);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "ArrayObject_GetSize");
    }

    return to_python(size);
}

PyObject* array_object_get_value(PyObject* self, PyObject* args) {
    PyObject* capsule;
    Py_ssize_t index;
    if (!PyArg_ParseTuple(args, "On", &capsule, &index)) {
        return nullptr;
    }

    size_type at = 0;
    if (safe_convert(index, &at) < 0) {
        return nullptr;
    }

    ArrayObjectHandle* array = object_cast<ArrayObjectHandle>(
        capsule, ArrayObject_FromObject, "ArrayObject_FromObject");
    if (array == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { ArrayObject_Release(array); });

    ObjectHandle* object = nullptr;
    error_type err = ArrayObject_GetValue(array, at, &object);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "ArrayObject_GetValue");
    }

    return object_capsule_from_handle(object);
}

PyObject* array_object_set_value(PyObject* self, PyObject* args) {
    PyObject* capsule;
    Py_ssize_t index;
    PyObject* value_capsule;
    if (!PyArg_ParseTuple(args, "OnO", &capsule, &index, &value_capsule)) {
        return nullptr;
    }

    size_type at = 0;
    if (safe_convert(index, &at) < 0) {
        return nullptr;
    }

    ObjectHandle* value = capsule_cast<ObjectHandle>(value_capsule, OBJECT_CAPSULE);
    if (value == nullptr) {
        return nullptr;
    }

    ArrayObjectHandle* array = object_cast<ArrayObjectHandle>(
        capsule, ArrayObject_FromObject, "ArrayObject_FromObject");
    if (array == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { ArrayObject_Release(array); });

    error_type err = ArrayObject_SetValue(array, at, value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "ArrayObject_SetValue");
    }

    Py_RETURN_NONE;
}

PyObject* array_object_append(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* value_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &value_capsule)) {
        return nullptr;
    }

    ObjectHandle* value = capsule_cast<ObjectHandle>(value_capsule, OBJECT_CAPSULE);
    if (value == nullptr) {
        return nullptr;
    }

    ArrayObjectHandle* array = object_cast<ArrayObjectHandle>(
        capsule, ArrayObject_FromObject, "ArrayObject_FromObject");
    if (array == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { ArrayObject_Release(array); });

    error_type err = ArrayObject_Append(array, value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "ArrayObject_Append");
    }

    Py_RETURN_NONE;
}

PyObject* array_object_insert(PyObject* self, PyObject* args) {
    PyObject* capsule;
    Py_ssize_t index;
    PyObject* value_capsule;
    if (!PyArg_ParseTuple(args, "OnO", &capsule, &index, &value_capsule)) {
        return nullptr;
    }

    size_type at = 0;
    if (safe_convert(index, &at) < 0) {
        return nullptr;
    }

    ObjectHandle* value = capsule_cast<ObjectHandle>(value_capsule, OBJECT_CAPSULE);
    if (value == nullptr) {
        return nullptr;
    }

    ArrayObjectHandle* array = object_cast<ArrayObjectHandle>(
        capsule, ArrayObject_FromObject, "ArrayObject_FromObject");
    if (array == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { ArrayObject_Release(array); });

    error_type err = ArrayObject_Insert(array, at, value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "ArrayObject_Insert");
    }

    Py_RETURN_NONE;
}

PyObject* array_object_remove(PyObject* self, PyObject* args) {
    PyObject* capsule;
    Py_ssize_t index;
    if (!PyArg_ParseTuple(args, "On", &capsule, &index)) {
        return nullptr;
    }

    size_type at = 0;
    if (safe_convert(index, &at) < 0) {
        return nullptr;
    }

    ArrayObjectHandle* array = object_cast<ArrayObjectHandle>(
        capsule, ArrayObject_FromObject, "ArrayObject_FromObject");
    if (array == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { ArrayObject_Release(array); });

    error_type err = ArrayObject_Remove(array, at);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "ArrayObject_Remove");
    }

    Py_RETURN_NONE;
}

PyObject* array_object_clear(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    ArrayObjectHandle* array = object_cast<ArrayObjectHandle>(
        capsule, ArrayObject_FromObject, "ArrayObject_FromObject");
    if (array == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { ArrayObject_Release(array); });

    error_type err = ArrayObject_Clear(array);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "ArrayObject_Clear");
    }

    Py_RETURN_NONE;
}
