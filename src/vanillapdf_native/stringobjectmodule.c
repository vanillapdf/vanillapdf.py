#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_buffer.h>
#include <vanillapdf/syntax/c_string_object.h>

#include "stringobjectmodule.h"

#define CAPSULE_NAME "VanillaPDF.StringObject"

typedef struct {
    StringObjectHandle* handle;  /* NULL => released */
} StringObjectHandleBox;

static void string_object_capsule_destructor(PyObject* capsule) {
    StringObjectHandleBox* box = (StringObjectHandleBox*)PyCapsule_GetPointer(
        capsule, CAPSULE_NAME);
    if (box != NULL) {
        if (box->handle != NULL) {
            StringObject_Release(box->handle);
        }
        PyMem_Free(box);
    }
}

static PyObject* string_object_capsule_from_handle(StringObjectHandle* handle) {
    StringObjectHandleBox* box =
        (StringObjectHandleBox*)PyMem_Malloc(sizeof(StringObjectHandleBox));
    if (box == NULL) {
        StringObject_Release(handle);
        return PyErr_NoMemory();
    }
    box->handle = handle;

    PyObject* capsule = PyCapsule_New(
        (void*)box, CAPSULE_NAME, string_object_capsule_destructor);
    if (capsule == NULL) {
        StringObject_Release(handle);
        PyMem_Free(box);
        return NULL;
    }
    return capsule;
}

PyObject* literal_string_object_create(PyObject* self, PyObject* args) {
    const char* data;
    Py_ssize_t size;
    if (!PyArg_ParseTuple(args, "y#", &data, &size)) {
        return NULL;
    }

    BufferHandle* buf = NULL;
    error_type err = Buffer_CreateFromData((string_type)data, (size_type)size, &buf);
    if (err != VANILLAPDF_ERROR_SUCCESS || buf == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create buffer");
        return NULL;
    }

    LiteralStringObjectHandle* lit = NULL;
    err = LiteralStringObject_CreateFromDecodedBuffer(buf, &lit);
    Buffer_Release(buf);
    if (err != VANILLAPDF_ERROR_SUCCESS || lit == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create literal string object");
        return NULL;
    }

    StringObjectHandle* so = NULL;
    err = LiteralStringObject_ToStringObject(lit, &so);
    LiteralStringObject_Release(lit);
    if (err != VANILLAPDF_ERROR_SUCCESS || so == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to upcast to string object");
        return NULL;
    }

    return string_object_capsule_from_handle(so);
}

PyObject* string_object_get_raw(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, CAPSULE_NAME)) {
        PyErr_SetString(PyExc_TypeError, "Invalid string object handle");
        return NULL;
    }

    StringObjectHandleBox* box =
        (StringObjectHandleBox*)PyCapsule_GetPointer(capsule, CAPSULE_NAME);
    if (box->handle == NULL) {
        PyErr_SetString(PyExc_ValueError, "StringObject has been released");
        return NULL;
    }

    BufferHandle* buf = NULL;
    error_type err = StringObject_GetValue(box->handle, &buf);
    if (err != VANILLAPDF_ERROR_SUCCESS || buf == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get string object value");
        return NULL;
    }

    string_type data = NULL;
    size_type size = 0;
    err = Buffer_GetData(buf, &data, &size);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        Buffer_Release(buf);
        PyErr_SetString(PyExc_RuntimeError, "Failed to read buffer data");
        return NULL;
    }

    PyObject* result = PyBytes_FromStringAndSize((const char*)data, (Py_ssize_t)size);
    Buffer_Release(buf);
    return result;
}

PyObject* string_object_set_raw(PyObject* self, PyObject* args) {
    PyObject* capsule;
    const char* data;
    Py_ssize_t size;
    if (!PyArg_ParseTuple(args, "Oy#", &capsule, &data, &size)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, CAPSULE_NAME)) {
        PyErr_SetString(PyExc_TypeError, "Invalid string object handle");
        return NULL;
    }

    StringObjectHandleBox* box =
        (StringObjectHandleBox*)PyCapsule_GetPointer(capsule, CAPSULE_NAME);
    if (box->handle == NULL) {
        PyErr_SetString(PyExc_ValueError, "StringObject has been released");
        return NULL;
    }

    BufferHandle* buf = NULL;
    error_type err = Buffer_CreateFromData((string_type)data, (size_type)size, &buf);
    if (err != VANILLAPDF_ERROR_SUCCESS || buf == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create buffer");
        return NULL;
    }

    err = StringObject_SetValue(box->handle, buf);
    Buffer_Release(buf);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to set string object value");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject* string_object_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, CAPSULE_NAME)) {
        PyErr_SetString(PyExc_TypeError, "Invalid string object handle");
        return NULL;
    }

    StringObjectHandleBox* box =
        (StringObjectHandleBox*)PyCapsule_GetPointer(capsule, CAPSULE_NAME);
    if (box->handle == NULL) {
        Py_RETURN_NONE;
    }

    StringObject_Release(box->handle);
    box->handle = NULL;

    Py_RETURN_NONE;
}
