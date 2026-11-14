#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_io_strategy.h>
#include <vanillapdf/syntax/c_file.h>
#include <vanillapdf/semantics/c_document.h>
#include <vanillapdf/semantics/c_catalog.h>
#include <vanillapdf/semantics/c_document_info.h>
#include <vanillapdf/semantics/c_document_encryption_settings.h>
#include <vanillapdf/semantics/c_document_signature_settings.h>

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
    int strategy;
    if (!PyArg_ParseTuple(args, "si", &filename, &strategy)) {
        return nullptr;
    }

    IOStrategyType strategy_value = static_cast<IOStrategyType>(strategy);
    DocumentHandle* handle = nullptr;
    error_type err = without_gil([filename, strategy_value, &handle] {
        return Document_OpenWithStrategy(filename, strategy_value, &handle);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_OpenWithStrategy");
    }

    return document_capsule_from_handle(handle);
}

PyObject* document_create(PyObject* self, PyObject* args) {
    const char* filename;
    int strategy;
    if (!PyArg_ParseTuple(args, "si", &filename, &strategy)) {
        return nullptr;
    }

    IOStrategyType strategy_value = static_cast<IOStrategyType>(strategy);
    DocumentHandle* handle = nullptr;
    error_type err = without_gil([filename, strategy_value, &handle] {
        return Document_CreateWithStrategy(filename, strategy_value, &handle);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_CreateWithStrategy");
    }

    return document_capsule_from_handle(handle);
}

PyObject* document_open_file(PyObject* self, PyObject* args) {
    PyObject* file_capsule;
    if (!PyArg_ParseTuple(args, "O", &file_capsule)) {
        return nullptr;
    }

    auto file = capsule_cast<FileHandle>(file_capsule, "VanillaPDF.File");
    if (!file) {
        return nullptr;
    }

    DocumentHandle* handle = nullptr;
    error_type err = without_gil([file = file.get(), &handle] {
        return Document_OpenFile(file, &handle);
    });
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

    auto handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    error_type err = without_gil([handle = handle.get(), filename] {
        return Document_Save(handle, filename);
    });
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

    auto handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    error_type err = without_gil([handle = handle.get(), filename] {
        return Document_SaveIncremental(handle, filename);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_SaveIncremental");
    }

    Py_RETURN_NONE;
}

PyObject* document_save_file(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* file_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &file_capsule)) {
        return nullptr;
    }

    auto handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    auto file = capsule_cast<FileHandle>(file_capsule, "VanillaPDF.File");
    if (!file) {
        return nullptr;
    }

    error_type err = without_gil([handle = handle.get(), file = file.get()] {
        return Document_SaveFile(handle, file);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_SaveFile");
    }

    Py_RETURN_NONE;
}

PyObject* document_save_incremental_file(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* file_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &file_capsule)) {
        return nullptr;
    }

    auto handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    auto file = capsule_cast<FileHandle>(file_capsule, "VanillaPDF.File");
    if (!file) {
        return nullptr;
    }

    error_type err = without_gil([handle = handle.get(), file = file.get()] {
        return Document_SaveIncrementalFile(handle, file);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_SaveIncrementalFile");
    }

    Py_RETURN_NONE;
}

PyObject* document_get_catalog(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    auto handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    CatalogHandle* catalog = nullptr;
    error_type err = without_gil([handle = handle.get(), &catalog] {
        return Document_GetCatalog(handle, &catalog);
    });
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

    auto handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    DocumentInfoHandle* info = nullptr;
    error_type err = without_gil([handle = handle.get(), &info] {
        return Document_GetDocumentInfo(handle, &info);
    });
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

    auto source = capsule_cast<DocumentHandle>(source_capsule, DOCUMENT_CAPSULE);
    if (!source) {
        return nullptr;
    }

    auto handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    error_type err = without_gil([handle = handle.get(), source = source.get()] {
        return Document_AppendDocument(handle, source);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_AppendDocument");
    }

    Py_RETURN_NONE;
}

PyObject* document_add_encryption(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* settings_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &settings_capsule)) {
        return nullptr;
    }

    auto settings =
        capsule_cast<DocumentEncryptionSettingsHandle>(
            settings_capsule, "VanillaPDF.DocumentEncryptionSettings");
    if (!settings) {
        return nullptr;
    }

    auto handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    error_type err = without_gil([handle = handle.get(), settings = settings.get()] {
        return Document_AddEncryption(handle, settings);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_AddEncryption");
    }

    Py_RETURN_NONE;
}

PyObject* document_remove_encryption(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    auto handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    error_type err = without_gil([handle = handle.get()] {
        return Document_RemoveEncryption(handle);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_RemoveEncryption");
    }

    Py_RETURN_NONE;
}

PyObject* document_sign(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* destination_capsule;
    PyObject* settings_capsule;
    if (!PyArg_ParseTuple(args, "OOO", &capsule, &destination_capsule, &settings_capsule)) {
        return nullptr;
    }

    auto destination =
        capsule_cast<FileHandle>(destination_capsule, "VanillaPDF.File");
    if (!destination) {
        return nullptr;
    }

    auto settings =
        capsule_cast<DocumentSignatureSettingsHandle>(
            settings_capsule, "VanillaPDF.DocumentSignatureSettings");
    if (!settings) {
        return nullptr;
    }

    auto handle = capsule_cast<DocumentHandle>(capsule, DOCUMENT_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    error_type err = without_gil([handle = handle.get(), destination = destination.get(), settings = settings.get()] {
        return Document_Sign(handle, destination, settings);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Document_Sign");
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
