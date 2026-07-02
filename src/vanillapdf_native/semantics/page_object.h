#ifndef VANILLAPDF_PY_SEMANTICS_PAGE_OBJECT_H
#define VANILLAPDF_PY_SEMANTICS_PAGE_OBJECT_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/semantics/c_page_object.h>

/* Wrap a PageObjectHandle in a capsule (takes ownership). */
PyObject* page_object_capsule_from_handle(PageObjectHandle* handle);

/* Page object ("VanillaPDF.PageObject" capsule). */
PyObject* page_object_create_from_document(PyObject* self, PyObject* args);
PyObject* page_object_get_media_box(PyObject* self, PyObject* args);
PyObject* page_object_set_media_box(PyObject* self, PyObject* args);
PyObject* page_object_get_base_object(PyObject* self, PyObject* args);
PyObject* page_object_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SEMANTICS_PAGE_OBJECT_H */
