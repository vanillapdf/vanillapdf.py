#ifndef VANILLAPDF_PY_UTILS_INPUT_OUTPUT_STREAM_H
#define VANILLAPDF_PY_UTILS_INPUT_OUTPUT_STREAM_H

#include <Python.h>

PyObject* input_output_stream_create_from_memory(PyObject* self, PyObject* args);
PyObject* input_output_stream_read_buffer(PyObject* self, PyObject* args);
PyObject* input_output_stream_get_output_position(PyObject* self, PyObject* args);
PyObject* input_output_stream_write_buffer(PyObject* self, PyObject* args);
PyObject* input_output_stream_flush(PyObject* self, PyObject* args);
PyObject* input_output_stream_release(PyObject* self, PyObject* args);

#endif // VANILLAPDF_PY_UTILS_INPUT_OUTPUT_STREAM_H
