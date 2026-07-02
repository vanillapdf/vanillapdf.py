#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_file.h>
#include <vanillapdf/semantics/c_document.h>
#include <vanillapdf/semantics/c_catalog.h>
#include <vanillapdf/semantics/c_document_info.h>

#include "semantics/document.h"
#include "semantics/catalog.h"
#include "semantics/document_info.h"
#include "common.h"

static const char* const DOCUMENT_CAPSULE = "VanillaPDF.Document";

static PyObject* document_capsule_from_handle(DocumentHandle* handle) {
    return capsule_new(handle, DOCUMENT_CAPSULE, RELEASE_FN(Document_Release));
}

PyObject* document_open(PyObject* self, PyObject* args) {
    const char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return nullptr;
    }

    DocumentHandle* handle = nullptr;
    error_type err = Document_Open(filename, &handle);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_Open");
    }

    return document_capsule_from_handle(handle);
}

PyObject* document_create(PyObject* self, PyObject* args) {
    const char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return nullptr;
    }

    DocumentHandle* handle = nullptr;
    error_type err = Document_Create(filename, &handle);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_Create");
    }

    return document_capsule_from_handle(handle);
}

PyObject* document_open_file(PyObject* self, PyObject* args) {
    PyObject* file_capsule;
    if (!PyArg_ParseTuple(args, "O", &file_capsule)) {
        return nullptr;
    }

    FileHandle* file = capsule_cast<FileHandle>(file_capsule, "VanillaPDF.File");
    if (file == nullptr) {
        return nullptr;
    }

    DocumentHandle* handle = nullptr;
    error_type err = Document_OpenFile(file, &handle);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_OpenFile");
    }

    return document_capsule_from_handle(handle);
}

PyObject* document_save(PyObject* self, PyObject* args) {
    PyObject* capsule;
    const char* filename;
    if (!PyArg_ParseTuple(args, "Os", &capsule, &filename)) {
        return nullptr;
    }

    DocumentHandle* handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    error_type err = Document_Save(handle, filename);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_Save");
    }

    Py_RETURN_NONE;
}

PyObject* document_save_incremental(PyObject* self, PyObject* args) {
    PyObject* capsule;
    const char* filename;
    if (!PyArg_ParseTuple(args, "Os", &capsule, &filename)) {
        return nullptr;
    }

    DocumentHandle* handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    error_type err = Document_SaveIncremental(handle, filename);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_SaveIncremental");
    }

    Py_RETURN_NONE;
}

PyObject* document_get_catalog(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DocumentHandle* handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    CatalogHandle* catalog = nullptr;
    error_type err = Document_GetCatalog(handle, &catalog);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_GetCatalog");
    }

    return catalog_capsule_from_handle(catalog);
}

PyObject* document_get_document_info(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DocumentHandle* handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    DocumentInfoHandle* info = nullptr;
    error_type err = Document_GetDocumentInfo(handle, &info);
    if (is_missing_error(err)) {
        Py_RETURN_NONE;
    }
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_GetDocumentInfo");
    }

    return document_info_capsule_from_handle(info);
}

PyObject* document_append_document(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* source_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &source_capsule)) {
        return nullptr;
    }

    DocumentHandle* source = capsule_cast<DocumentHandle>(source_capsule, DOCUMENT_CAPSULE);
    if (source == nullptr) {
        return nullptr;
    }

    DocumentHandle* handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    error_type err = Document_AppendDocument(handle, source);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_AppendDocument");
    }

    Py_RETURN_NONE;
}

PyObject* document_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, DOCUMENT_CAPSULE);
}
