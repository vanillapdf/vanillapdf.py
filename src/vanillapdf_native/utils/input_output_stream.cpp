#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_buffer.h>
#include <vanillapdf/utils/c_input_output_stream.h>

#include "utils/input_output_stream.h"
#include "utils/buffer.h"
#include "common.h"

static const char* const INPUT_OUTPUT_STREAM_CAPSULE = "VanillaPDF.InputOutputStream";

PyObject* input_output_stream_create_from_memory(PyObject* self, PyObject* args) {
    InputOutputStreamHandle* handle = nullptr;
    error_type err = without_gil([&handle] {
        return InputOutputStream_CreateFromMemory(&handle);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "InputOutputStream_CreateFromMemory");
    }

    return capsule_new(handle, INPUT_OUTPUT_STREAM_CAPSULE, RELEASE_FN(InputOutputStream_Release));
}

PyObject* input_output_stream_read_buffer(PyObject* self, PyObject* args) {
    PyObject* capsule;
    long long length;
    if (!PyArg_ParseTuple(args, "OL", &capsule, &length)) {
        return nullptr;
    }

    InputOutputStreamHandle* handle =
        capsule_cast<InputOutputStreamHandle>(capsule, INPUT_OUTPUT_STREAM_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    bigint_type read_length = 0;
    if (safe_convert(length, &read_length) < 0) {
        return nullptr;
    }

    BufferHandle* buffer = nullptr;
    error_type err = without_gil([handle, read_length, &buffer] {
        return InputOutputStream_ReadBuffer(handle, read_length, &buffer);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "InputOutputStream_ReadBuffer");
    }

    return buffer_capsule_from_handle(buffer);
}

PyObject* input_output_stream_get_input_position(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    InputOutputStreamHandle* handle =
        capsule_cast<InputOutputStreamHandle>(capsule, INPUT_OUTPUT_STREAM_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    offset_type value = 0;
    error_type err = without_gil([handle, &value] {
        return InputOutputStream_GetInputPosition(handle, &value);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "InputOutputStream_GetInputPosition");
    }

    return to_python(value);
}

PyObject* input_output_stream_set_input_position(PyObject* self, PyObject* args) {
    PyObject* capsule;
    long long position;
    if (!PyArg_ParseTuple(args, "OL", &capsule, &position)) {
        return nullptr;
    }

    InputOutputStreamHandle* handle =
        capsule_cast<InputOutputStreamHandle>(capsule, INPUT_OUTPUT_STREAM_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    offset_type value = 0;
    if (safe_convert(position, &value) < 0) {
        return nullptr;
    }

    error_type err = without_gil([handle, value] {
        return InputOutputStream_SetInputPosition(handle, value);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "InputOutputStream_SetInputPosition");
    }

    Py_RETURN_NONE;
}

PyObject* input_output_stream_get_output_position(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    InputOutputStreamHandle* handle =
        capsule_cast<InputOutputStreamHandle>(capsule, INPUT_OUTPUT_STREAM_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    offset_type value = 0;
    error_type err = without_gil([handle, &value] {
        return InputOutputStream_GetOutputPosition(handle, &value);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "InputOutputStream_GetOutputPosition");
    }

    return to_python(value);
}

PyObject* input_output_stream_write_buffer(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* buffer_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &buffer_capsule)) {
        return nullptr;
    }

    InputOutputStreamHandle* handle =
        capsule_cast<InputOutputStreamHandle>(capsule, INPUT_OUTPUT_STREAM_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    BufferHandle* buffer = capsule_cast<BufferHandle>(buffer_capsule, "VanillaPDF.Buffer");
    if (buffer == nullptr) {
        return nullptr;
    }

    error_type err = without_gil([handle, buffer] {
        return InputOutputStream_WriteBuffer(handle, buffer);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "InputOutputStream_WriteBuffer");
    }

    Py_RETURN_NONE;
}

PyObject* input_output_stream_flush(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    InputOutputStreamHandle* handle =
        capsule_cast<InputOutputStreamHandle>(capsule, INPUT_OUTPUT_STREAM_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    error_type err = without_gil([handle] {
        return InputOutputStream_Flush(handle);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "InputOutputStream_Flush");
    }

    Py_RETURN_NONE;
}

PyObject* input_output_stream_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, INPUT_OUTPUT_STREAM_CAPSULE);
}
