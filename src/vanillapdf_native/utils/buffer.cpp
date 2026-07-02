#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_buffer.h>

#include "utils/buffer.h"
#include "common.h"

static const char* const BUFFER_CAPSULE = "VanillaPDF.Buffer";

PyObject* buffer_capsule_from_handle(BufferHandle* handle) {
    return capsule_new(handle, BUFFER_CAPSULE, RELEASE_FN(Buffer_Release));
}

PyObject* buffer_create(PyObject* self, PyObject* args) {
    BufferHandle* handle = nullptr;
    error_type err = Buffer_Create(&handle);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Buffer_Create");
    }

    return capsule_new(handle, BUFFER_CAPSULE, RELEASE_FN(Buffer_Release));
}

PyObject* buffer_create_from_data(PyObject* self, PyObject* args) {
    const char* data;
    Py_ssize_t size;
    if (!PyArg_ParseTuple(args, "y#", &data, &size)) {
        return nullptr;
    }

    size_type data_size = 0;
    if (safe_convert(size, &data_size) < 0) {
        return nullptr;
    }

    BufferHandle* handle = nullptr;
    error_type err = Buffer_CreateFromData(data, data_size, &handle);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Buffer_CreateFromData");
    }

    return capsule_new(handle, BUFFER_CAPSULE, RELEASE_FN(Buffer_Release));
}

PyObject* buffer_set_data(PyObject* self, PyObject* args) {
    PyObject* capsule;
    const char* data;
    Py_ssize_t size;
    if (!PyArg_ParseTuple(args, "Oy#", &capsule, &data, &size)) {
        return nullptr;
    }

    BufferHandle* handle = capsule_cast<BufferHandle>(capsule, BUFFER_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    size_type data_size = 0;
    if (safe_convert(size, &data_size) < 0) {
        return nullptr;
    }

    error_type err = Buffer_SetData(handle, data, data_size);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Buffer_SetData");
    }

    Py_RETURN_NONE;
}

PyObject* buffer_get_data(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    BufferHandle* handle = capsule_cast<BufferHandle>(capsule, BUFFER_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    string_type data = nullptr;
    size_type size = 0;
    error_type err = Buffer_GetData(handle, &data, &size);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Buffer_GetData");
    }

    Py_ssize_t length = 0;
    if (safe_convert(size, &length) < 0) {
        return nullptr;
    }

    return PyBytes_FromStringAndSize(data, length);
}

PyObject* buffer_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, BUFFER_CAPSULE);
}
