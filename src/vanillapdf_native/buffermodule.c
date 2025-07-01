#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_buffer.h>

#include "buffermodule.h"

PyObject* vp_buffer_create(PyObject* self, PyObject* args) {
    BufferHandle* handle = NULL;
    error_type err = Buffer_Create(&handle);

    if (err != VANILLAPDF_ERROR_SUCCESS || handle == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create buffer");
        return NULL;
    }

    return PyCapsule_New((void*)handle, "VanillaPDF.Buffer", NULL);
}

PyObject* vp_buffer_create_from_data(PyObject* self, PyObject* args) {
    const char* data;
    Py_ssize_t size;
    if (!PyArg_ParseTuple(args, "y#", &data, &size)) {
        return NULL;
    }

    BufferHandle* handle = NULL;
    error_type err = Buffer_CreateFromData((string_type)data, (size_type)size, &handle);

    if (err != VANILLAPDF_ERROR_SUCCESS || handle == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create buffer from data");
        return NULL;
    }

    return PyCapsule_New((void*)handle, "VanillaPDF.Buffer", NULL);
}

PyObject* vp_buffer_set_data(PyObject* self, PyObject* args) {
    PyObject* capsule;
    const char* data;
    Py_ssize_t size;
    if (!PyArg_ParseTuple(args, "Oy#", &capsule, &data, &size)) {
        return NULL;
    }

    BufferHandle* handle = (BufferHandle*)PyCapsule_GetPointer(capsule, "VanillaPDF.Buffer");
    if (!handle) {
        return NULL;
    }

    error_type err = Buffer_SetData(handle, (string_type)data, (size_type)size);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to set buffer data");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject* vp_buffer_get_data(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }

    BufferHandle* handle = (BufferHandle*)PyCapsule_GetPointer(capsule, "VanillaPDF.Buffer");
    if (!handle) {
        return NULL;
    }

    string_type data = NULL;
    size_type size = 0;
    error_type err = Buffer_GetData(handle, &data, &size);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get buffer data");
        return NULL;
    }

    return PyBytes_FromStringAndSize((const char*)data, (Py_ssize_t)size);
}

PyObject* vp_buffer_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }

    BufferHandle* handle = (BufferHandle*)PyCapsule_GetPointer(capsule, "VanillaPDF.Buffer");
    if (!handle) {
        return NULL;
    }

    Buffer_Release(handle);
    Py_RETURN_NONE;
}


