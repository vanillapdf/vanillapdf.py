#ifndef VANILLAPDF_PY_SEMANTICS_CATALOG_H
#define VANILLAPDF_PY_SEMANTICS_CATALOG_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/semantics/c_catalog.h>

/* Wrap a CatalogHandle in a capsule (takes ownership). */
PyObject* catalog_capsule_from_handle(CatalogHandle* handle);

/* Document catalog ("VanillaPDF.Catalog" capsule). */
PyObject* catalog_get_pages(PyObject* self, PyObject* args);
PyObject* catalog_get_acro_form(PyObject* self, PyObject* args);
PyObject* catalog_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SEMANTICS_CATALOG_H */
