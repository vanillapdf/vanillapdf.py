#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_buffer.h>

#include "buffermodule.h"
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

    BufferHandle* handle = nullptr;
    error_type err = Buffer_CreateFromData(data, (size_type)size, &handle);
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

    error_type err = Buffer_SetData(handle, data, (size_type)size);
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

    return PyBytes_FromStringAndSize(data, (Py_ssize_t)size);
}

PyObject* buffer_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, BUFFER_CAPSULE);
}
