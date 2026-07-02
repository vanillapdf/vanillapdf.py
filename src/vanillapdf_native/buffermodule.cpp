#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_buffer.h>

#include "buffermodule.h"

static void buffer_capsule_destructor(PyObject* capsule) {
    BufferHandleBox* box = (BufferHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.Buffer");
    if (box != NULL) {
        if (box->handle != NULL) {
            Buffer_Release(box->handle);
        }
        PyMem_Free(box);
    }
}

PyObject* buffer_capsule_from_handle(BufferHandle* handle) {
    if (handle == NULL) {
        PyErr_SetString(PyExc_ValueError, "Cannot create capsule from NULL handle");
        return NULL;
    }

    BufferHandleBox* box = (BufferHandleBox*)PyMem_Malloc(sizeof(BufferHandleBox));
    if (box == NULL) {
        Buffer_Release(handle);
        return PyErr_NoMemory();
    }
    box->handle = handle;

    PyObject* capsule = PyCapsule_New((void*)box, "VanillaPDF.Buffer",
                                       buffer_capsule_destructor);
    if (capsule == NULL) {
        Buffer_Release(handle);
        PyMem_Free(box);
        return NULL;
    }

    return capsule;
}

PyObject* buffer_create(PyObject* self, PyObject* args) {
    BufferHandle* handle = NULL;
    error_type err = Buffer_Create(&handle);

    if (err != VANILLAPDF_ERROR_SUCCESS || handle == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create buffer");
        return NULL;
    }

    BufferHandleBox* box = (BufferHandleBox*)PyMem_Malloc(sizeof(BufferHandleBox));
    if (box == NULL) {
        Buffer_Release(handle);
        return PyErr_NoMemory();
    }
    box->handle = handle;

    PyObject* capsule = PyCapsule_New((void*)box, "VanillaPDF.Buffer",
                                       buffer_capsule_destructor);
    if (capsule == NULL) {
        Buffer_Release(handle);
        PyMem_Free(box);
        return NULL;
    }

    return capsule;
}

PyObject* buffer_create_from_data(PyObject* self, PyObject* args) {
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

    BufferHandleBox* box = (BufferHandleBox*)PyMem_Malloc(sizeof(BufferHandleBox));
    if (box == NULL) {
        Buffer_Release(handle);
        return PyErr_NoMemory();
    }
    box->handle = handle;

    PyObject* capsule = PyCapsule_New((void*)box, "VanillaPDF.Buffer",
                                       buffer_capsule_destructor);
    if (capsule == NULL) {
        Buffer_Release(handle);
        PyMem_Free(box);
        return NULL;
    }

    return capsule;
}

PyObject* buffer_set_data(PyObject* self, PyObject* args) {
    PyObject* capsule;
    const char* data;
    Py_ssize_t size;
    if (!PyArg_ParseTuple(args, "Oy#", &capsule, &data, &size)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, "VanillaPDF.Buffer")) {
        PyErr_SetString(PyExc_TypeError, "Invalid buffer handle");
        return NULL;
    }

    BufferHandleBox* box = (BufferHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.Buffer");
    if (box->handle == NULL) {
        PyErr_SetString(PyExc_ValueError, "Buffer has been released");
        return NULL;
    }

    error_type err = Buffer_SetData(box->handle, (string_type)data, (size_type)size);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to set buffer data");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject* buffer_get_data(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, "VanillaPDF.Buffer")) {
        PyErr_SetString(PyExc_TypeError, "Invalid buffer handle");
        return NULL;
    }

    BufferHandleBox* box = (BufferHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.Buffer");
    if (box->handle == NULL) {
        PyErr_SetString(PyExc_ValueError, "Buffer has been released");
        return NULL;
    }

    string_type data = NULL;
    size_type size = 0;
    error_type err = Buffer_GetData(box->handle, &data, &size);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get buffer data");
        return NULL;
    }

    return PyBytes_FromStringAndSize((const char*)data, (Py_ssize_t)size);
}

PyObject* buffer_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, "VanillaPDF.Buffer")) {
        PyErr_SetString(PyExc_TypeError, "Invalid buffer handle");
        return NULL;
    }

    BufferHandleBox* box = (BufferHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.Buffer");
    if (box->handle == NULL) {
        /* Already released - return success to make release idempotent */
        Py_RETURN_NONE;
    }

    Buffer_Release(box->handle);
    box->handle = NULL;

    Py_RETURN_NONE;
}
