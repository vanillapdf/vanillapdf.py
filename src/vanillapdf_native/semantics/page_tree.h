#ifndef VANILLAPDF_PY_SEMANTICS_PAGE_TREE_H
#define VANILLAPDF_PY_SEMANTICS_PAGE_TREE_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/semantics/c_page_tree.h>

/* Wrap a PageTreeHandle in a capsule (takes ownership). */
PyObject* page_tree_capsule_from_handle(PageTreeHandle* handle);

/* Page tree ("VanillaPDF.PageTree" capsule). */
PyObject* page_tree_get_page_count(PyObject* self, PyObject* args);
PyObject* page_tree_get_page(PyObject* self, PyObject* args);
PyObject* page_tree_insert_page(PyObject* self, PyObject* args);
PyObject* page_tree_append_page(PyObject* self, PyObject* args);
PyObject* page_tree_remove_page(PyObject* self, PyObject* args);
PyObject* page_tree_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SEMANTICS_PAGE_TREE_H */
