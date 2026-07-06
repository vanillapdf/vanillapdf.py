#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_buffer.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/syntax/c_dictionary_object.h>
#include <vanillapdf/syntax/c_stream_object.h>

#include "syntax/stream_object.h"
#include "syntax/object.h"
#include "utils/buffer.h"
#include "common.h"

PyObject* stream_object_create(PyObject* self, PyObject* args) {
    StreamObjectHandle* stream = nullptr;
    error_type create_err = StreamObject_Create(&stream);
    if (create_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(create_err, "StreamObject_Create");
    }
    SCOPE_GUARD([stream] { StreamObject_Release(stream); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = StreamObject_ToObject(stream, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "StreamObject_ToObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* stream_object_get_header(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    StreamObjectHandle* stream = object_cast<StreamObjectHandle>(
        capsule, StreamObject_FromObject, "StreamObject_FromObject");
    if (stream == nullptr) {
        return nullptr;
    }
    SCOPE_GUARD([stream] { StreamObject_Release(stream); });

    DictionaryObjectHandle* header = nullptr;
    error_type header_err = StreamObject_GetHeader(stream, &header);
    if (header_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(header_err, "StreamObject_GetHeader");
    }
    SCOPE_GUARD([header] { DictionaryObject_Release(header); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = DictionaryObject_ToObject(header, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "DictionaryObject_ToObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* stream_object_set_header(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* header_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &header_capsule)) {
        return nullptr;
    }

    DictionaryObjectHandle* header = object_cast<DictionaryObjectHandle>(
        header_capsule, DictionaryObject_FromObject, "DictionaryObject_FromObject");
    if (header == nullptr) {
        return nullptr;
    }
    SCOPE_GUARD([header] { DictionaryObject_Release(header); });

    StreamObjectHandle* stream = object_cast<StreamObjectHandle>(
        capsule, StreamObject_FromObject, "StreamObject_FromObject");
    if (stream == nullptr) {
        return nullptr;
    }
    SCOPE_GUARD([stream] { StreamObject_Release(stream); });

    error_type err = StreamObject_SetHeader(stream, header);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "StreamObject_SetHeader");
    }

    Py_RETURN_NONE;
}

PyObject* stream_object_get_body(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    StreamObjectHandle* stream = object_cast<StreamObjectHandle>(
        capsule, StreamObject_FromObject, "StreamObject_FromObject");
    if (stream == nullptr) {
        return nullptr;
    }
    SCOPE_GUARD([stream] { StreamObject_Release(stream); });

    BufferHandle* buffer = nullptr;
    error_type err = StreamObject_GetBody(stream, &buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "StreamObject_GetBody");
    }

    return buffer_capsule_from_handle(buffer);
}

PyObject* stream_object_get_body_raw(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    StreamObjectHandle* stream = object_cast<StreamObjectHandle>(
        capsule, StreamObject_FromObject, "StreamObject_FromObject");
    if (stream == nullptr) {
        return nullptr;
    }
    SCOPE_GUARD([stream] { StreamObject_Release(stream); });

    BufferHandle* buffer = nullptr;
    error_type err = StreamObject_GetBodyRaw(stream, &buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "StreamObject_GetBodyRaw");
    }

    return buffer_capsule_from_handle(buffer);
}

PyObject* stream_object_set_body(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* buffer_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &buffer_capsule)) {
        return nullptr;
    }

    BufferHandle* buffer = capsule_cast<BufferHandle>(buffer_capsule, "VanillaPDF.Buffer");
    if (buffer == nullptr) {
        return nullptr;
    }

    StreamObjectHandle* stream = object_cast<StreamObjectHandle>(
        capsule, StreamObject_FromObject, "StreamObject_FromObject");
    if (stream == nullptr) {
        return nullptr;
    }
    SCOPE_GUARD([stream] { StreamObject_Release(stream); });

    error_type err = StreamObject_SetBody(stream, buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "StreamObject_SetBody");
    }

    Py_RETURN_NONE;
}
