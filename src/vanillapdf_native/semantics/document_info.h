#ifndef VANILLAPDF_PY_SEMANTICS_DOCUMENT_INFO_H
#define VANILLAPDF_PY_SEMANTICS_DOCUMENT_INFO_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/semantics/c_document_info.h>

/* Wrap a DocumentInfoHandle in a capsule (takes ownership). */
PyObject* document_info_capsule_from_handle(DocumentInfoHandle* handle);

/* Document info ("VanillaPDF.DocumentInfo" capsule). Each getter returns a
 * String Object, or None when the entry is absent. */
PyObject* document_info_get_title(PyObject* self, PyObject* args);
PyObject* document_info_get_author(PyObject* self, PyObject* args);
PyObject* document_info_get_subject(PyObject* self, PyObject* args);
PyObject* document_info_get_keywords(PyObject* self, PyObject* args);
PyObject* document_info_get_creator(PyObject* self, PyObject* args);
PyObject* document_info_get_producer(PyObject* self, PyObject* args);
PyObject* document_info_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SEMANTICS_DOCUMENT_INFO_H */
