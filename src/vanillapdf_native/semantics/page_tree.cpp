#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/semantics/c_page_tree.h>
#include <vanillapdf/semantics/c_page_object.h>

#include "semantics/page_tree.h"
#include "semantics/page_object.h"
#include "common.h"

static const char* const PAGE_TREE_CAPSULE = "VanillaPDF.PageTree";

PyObject* page_tree_capsule_from_handle(PageTreeHandle* handle) {
    return capsule_new(handle, PAGE_TREE_CAPSULE, RELEASE_FN(PageTree_Release));
}

PyObject* page_tree_get_page_count(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    PageTreeHandle* page_tree = capsule_cast<PageTreeHandle>(capsule, PAGE_TREE_CAPSULE);
    if (page_tree == nullptr) {
        return nullptr;
    }

    size_type count = 0;
    error_type err = PageTree_GetPageCount(page_tree, &count);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "PageTree_GetPageCount");
    }

    return to_python(count);
}

PyObject* page_tree_get_page(PyObject* self, PyObject* args) {
    PyObject* capsule;
    Py_ssize_t index;
    if (!PyArg_ParseTuple(args, "On", &capsule, &index)) {
        return nullptr;
    }

    size_type at = 0;
    if (safe_convert(index, &at) < 0) {
        return nullptr;
    }

    PageTreeHandle* page_tree = capsule_cast<PageTreeHandle>(capsule, PAGE_TREE_CAPSULE);
    if (page_tree == nullptr) {
        return nullptr;
    }

    PageObjectHandle* page = nullptr;
    error_type err = PageTree_GetPage(page_tree, at, &page);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "PageTree_GetPage");
    }

    return page_object_capsule_from_handle(page);
}

PyObject* page_tree_insert_page(PyObject* self, PyObject* args) {
    PyObject* capsule;
    Py_ssize_t index;
    PyObject* page_capsule;
    if (!PyArg_ParseTuple(args, "OnO", &capsule, &index, &page_capsule)) {
        return nullptr;
    }

    size_type at = 0;
    if (safe_convert(index, &at) < 0) {
        return nullptr;
    }

    PageObjectHandle* page = capsule_cast<PageObjectHandle>(page_capsule, "VanillaPDF.PageObject");
    if (page == nullptr) {
        return nullptr;
    }

    PageTreeHandle* page_tree = capsule_cast<PageTreeHandle>(capsule, PAGE_TREE_CAPSULE);
    if (page_tree == nullptr) {
        return nullptr;
    }

    error_type err = PageTree_InsertPage(page_tree, at, page);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "PageTree_InsertPage");
    }

    Py_RETURN_NONE;
}

PyObject* page_tree_append_page(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* page_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &page_capsule)) {
        return nullptr;
    }

    PageObjectHandle* page = capsule_cast<PageObjectHandle>(page_capsule, "VanillaPDF.PageObject");
    if (page == nullptr) {
        return nullptr;
    }

    PageTreeHandle* page_tree = capsule_cast<PageTreeHandle>(capsule, PAGE_TREE_CAPSULE);
    if (page_tree == nullptr) {
        return nullptr;
    }

    error_type err = PageTree_AppendPage(page_tree, page);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "PageTree_AppendPage");
    }

    Py_RETURN_NONE;
}

PyObject* page_tree_remove_page(PyObject* self, PyObject* args) {
    PyObject* capsule;
    Py_ssize_t index;
    if (!PyArg_ParseTuple(args, "On", &capsule, &index)) {
        return nullptr;
    }

    size_type at = 0;
    if (safe_convert(index, &at) < 0) {
        return nullptr;
    }

    PageTreeHandle* page_tree = capsule_cast<PageTreeHandle>(capsule, PAGE_TREE_CAPSULE);
    if (page_tree == nullptr) {
        return nullptr;
    }

    error_type err = PageTree_RemovePage(page_tree, at);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "PageTree_RemovePage");
    }

    Py_RETURN_NONE;
}

PyObject* page_tree_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, PAGE_TREE_CAPSULE);
}
