#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_buffer.h>
#include <vanillapdf/semantics/c_document_encryption_settings.h>

#include "semantics/document_encryption_settings.h"
#include "common.h"

static const char* const SETTINGS_CAPSULE = "VanillaPDF.DocumentEncryptionSettings";

static PyObject* settings_capsule_from_handle(DocumentEncryptionSettingsHandle* handle) {
    return capsule_new(handle, SETTINGS_CAPSULE, RELEASE_FN(DocumentEncryptionSettings_Release));
}

PyObject* document_encryption_settings_create(PyObject* self, PyObject* args) {
    DocumentEncryptionSettingsHandle* settings = nullptr;
    error_type err = DocumentEncryptionSettings_Create(&settings);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DocumentEncryptionSettings_Create");
    }

    return settings_capsule_from_handle(settings);
}

PyObject* document_encryption_settings_get_algorithm(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DocumentEncryptionSettingsHandle* settings =
        capsule_cast<DocumentEncryptionSettingsHandle>(capsule, SETTINGS_CAPSULE);
    if (settings == nullptr) {
        return nullptr;
    }

    EncryptionAlgorithmType algorithm = EncryptionAlgorithmType_Undefined;
    error_type err = DocumentEncryptionSettings_GetAlgorithm(settings, &algorithm);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DocumentEncryptionSettings_GetAlgorithm");
    }

    return to_python(algorithm);
}

PyObject* document_encryption_settings_set_algorithm(PyObject* self, PyObject* args) {
    PyObject* capsule;
    int algorithm;
    if (!PyArg_ParseTuple(args, "Oi", &capsule, &algorithm)) {
        return nullptr;
    }

    DocumentEncryptionSettingsHandle* settings =
        capsule_cast<DocumentEncryptionSettingsHandle>(capsule, SETTINGS_CAPSULE);
    if (settings == nullptr) {
        return nullptr;
    }

    EncryptionAlgorithmType value = static_cast<EncryptionAlgorithmType>(algorithm);
    error_type err = DocumentEncryptionSettings_SetAlgorithm(settings, value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DocumentEncryptionSettings_SetAlgorithm");
    }

    Py_RETURN_NONE;
}

PyObject* document_encryption_settings_get_key_length(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DocumentEncryptionSettingsHandle* settings =
        capsule_cast<DocumentEncryptionSettingsHandle>(capsule, SETTINGS_CAPSULE);
    if (settings == nullptr) {
        return nullptr;
    }

    integer_type key_length = 0;
    error_type err = DocumentEncryptionSettings_GetKeyLength(settings, &key_length);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DocumentEncryptionSettings_GetKeyLength");
    }

    return to_python(key_length);
}

PyObject* document_encryption_settings_set_key_length(PyObject* self, PyObject* args) {
    PyObject* capsule;
    int key_length;
    if (!PyArg_ParseTuple(args, "Oi", &capsule, &key_length)) {
        return nullptr;
    }

    DocumentEncryptionSettingsHandle* settings =
        capsule_cast<DocumentEncryptionSettingsHandle>(capsule, SETTINGS_CAPSULE);
    if (settings == nullptr) {
        return nullptr;
    }

    error_type err = DocumentEncryptionSettings_SetKeyLength(settings, key_length);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DocumentEncryptionSettings_SetKeyLength");
    }

    Py_RETURN_NONE;
}

PyObject* document_encryption_settings_get_user_access_permissions(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DocumentEncryptionSettingsHandle* settings =
        capsule_cast<DocumentEncryptionSettingsHandle>(capsule, SETTINGS_CAPSULE);
    if (settings == nullptr) {
        return nullptr;
    }

    UserAccessPermissionFlags permissions = UserAccessPermissionFlag_None;
    error_type err = DocumentEncryptionSettings_GetUserAccessPermissions(settings, &permissions);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DocumentEncryptionSettings_GetUserAccessPermissions");
    }

    return to_python(permissions);
}

PyObject* document_encryption_settings_set_user_access_permissions(PyObject* self, PyObject* args) {
    PyObject* capsule;
    int permissions;
    if (!PyArg_ParseTuple(args, "Oi", &capsule, &permissions)) {
        return nullptr;
    }

    DocumentEncryptionSettingsHandle* settings =
        capsule_cast<DocumentEncryptionSettingsHandle>(capsule, SETTINGS_CAPSULE);
    if (settings == nullptr) {
        return nullptr;
    }

    UserAccessPermissionFlags value = static_cast<UserAccessPermissionFlags>(permissions);
    error_type err = DocumentEncryptionSettings_SetUserAccessPermissions(settings, value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DocumentEncryptionSettings_SetUserAccessPermissions");
    }

    Py_RETURN_NONE;
}

/* Shared body for the two Buffer-typed password setters. */
static PyObject* set_password(
        PyObject* args,
        error_type (CALLING_CONVENTION *setter)(DocumentEncryptionSettingsHandle*, BufferHandle*),
        const char* operation) {
    PyObject* capsule;
    PyObject* buffer_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &buffer_capsule)) {
        return nullptr;
    }

    DocumentEncryptionSettingsHandle* settings =
        capsule_cast<DocumentEncryptionSettingsHandle>(capsule, SETTINGS_CAPSULE);
    if (settings == nullptr) {
        return nullptr;
    }

    BufferHandle* buffer = capsule_cast<BufferHandle>(buffer_capsule, "VanillaPDF.Buffer");
    if (buffer == nullptr) {
        return nullptr;
    }

    error_type err = setter(settings, buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, operation);
    }

    Py_RETURN_NONE;
}

PyObject* document_encryption_settings_set_user_password(PyObject* self, PyObject* args) {
    return set_password(args, DocumentEncryptionSettings_SetUserPassword,
                        "DocumentEncryptionSettings_SetUserPassword");
}

PyObject* document_encryption_settings_set_owner_password(PyObject* self, PyObject* args) {
    return set_password(args, DocumentEncryptionSettings_SetOwnerPassword,
                        "DocumentEncryptionSettings_SetOwnerPassword");
}

PyObject* document_encryption_settings_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, SETTINGS_CAPSULE);
}
