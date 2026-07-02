#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/semantics/c_catalog.h>
#include <vanillapdf/semantics/c_page_tree.h>

#include "semantics/catalog.h"
#include "semantics/page_tree.h"
#include "common.h"

static const char* const CATALOG_CAPSULE = "VanillaPDF.Catalog";

PyObject* catalog_capsule_from_handle(CatalogHandle* handle) {
    return capsule_new(handle, CATALOG_CAPSULE, RELEASE_FN(Catalog_Release));
}

PyObject* catalog_get_pages(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    CatalogHandle* catalog = capsule_cast<CatalogHandle>(capsule, CATALOG_CAPSULE);
    if (catalog == nullptr) {
        return nullptr;
    }

    PageTreeHandle* pages = nullptr;
    error_type err = Catalog_GetPages(catalog, &pages);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Catalog_GetPages");
    }

    return page_tree_capsule_from_handle(pages);
}

PyObject* catalog_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, CATALOG_CAPSULE);
}
