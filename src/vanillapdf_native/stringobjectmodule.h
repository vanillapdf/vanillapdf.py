#ifndef VANILLAPDF_STRING_OBJECT_MODULE_H
#define VANILLAPDF_STRING_OBJECT_MODULE_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

/* Create a LiteralStringObject from raw decoded bytes.
   Python signature: literal_string_object_create(data: bytes) -> capsule
   The bytes are treated as the decoded PDF text string content (may contain
   BOM for UTF-16BE or UTF-8, or be bare PDFDocEncoding bytes). */
PyObject* literal_string_object_create(PyObject* self, PyObject* args);

/* Decode the StringObject's text to a Python str.
   Python signature: string_object_get_text(capsule) -> str
   Detects encoding automatically (PDFDocEncoding, UTF-16BE, UTF-8). */
PyObject* string_object_get_text(PyObject* self, PyObject* args);

/* Set the StringObject's text from a Python str.
   Python signature: string_object_set_text(capsule, text: str) -> None
   Always stores as UTF-16BE with BOM, covering the full Unicode range. */
PyObject* string_object_set_text(PyObject* self, PyObject* args);

/* Get the StringObject's raw decoded bytes (for test verification).
   Python signature: string_object_get_raw(capsule) -> bytes */
PyObject* string_object_get_raw(PyObject* self, PyObject* args);

/* Release the StringObject handle.
   Python signature: string_object_release(capsule) -> None */
PyObject* string_object_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_STRING_OBJECT_MODULE_H */
