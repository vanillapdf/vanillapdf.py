#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_signing_key.h>
#include <vanillapdf/utils/c_message_digest_algorithm.h>
#include <vanillapdf/semantics/c_document_signature_settings.h>

#include "semantics/document_signature_settings.h"
#include "common.h"

static const char* const SETTINGS_CAPSULE = "VanillaPDF.DocumentSignatureSettings";

static PyObject* settings_capsule_from_handle(DocumentSignatureSettingsHandle* handle) {
    return capsule_new(handle, SETTINGS_CAPSULE, RELEASE_FN(DocumentSignatureSettings_Release));
}

PyObject* document_signature_settings_create(PyObject* self, PyObject* args) {
    DocumentSignatureSettingsHandle* settings = nullptr;
    error_type err = DocumentSignatureSettings_Create(&settings);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DocumentSignatureSettings_Create");
    }

    return settings_capsule_from_handle(settings);
}

PyObject* document_signature_settings_get_digest(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DocumentSignatureSettingsHandle* settings =
        capsule_cast<DocumentSignatureSettingsHandle>(capsule, SETTINGS_CAPSULE);
    if (settings == nullptr) {
        return nullptr;
    }

    MessageDigestAlgorithmType digest = MessageDigestAlgorithmType_Undefined;
    error_type err = DocumentSignatureSettings_GetDigest(settings, &digest);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DocumentSignatureSettings_GetDigest");
    }

    return to_python(digest);
}

PyObject* document_signature_settings_set_digest(PyObject* self, PyObject* args) {
    PyObject* capsule;
    int digest;
    if (!PyArg_ParseTuple(args, "Oi", &capsule, &digest)) {
        return nullptr;
    }

    DocumentSignatureSettingsHandle* settings =
        capsule_cast<DocumentSignatureSettingsHandle>(capsule, SETTINGS_CAPSULE);
    if (settings == nullptr) {
        return nullptr;
    }

    MessageDigestAlgorithmType value = static_cast<MessageDigestAlgorithmType>(digest);
    error_type err = DocumentSignatureSettings_SetDigest(settings, value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DocumentSignatureSettings_SetDigest");
    }

    Py_RETURN_NONE;
}

PyObject* document_signature_settings_set_signing_key(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* signing_key_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &signing_key_capsule)) {
        return nullptr;
    }

    DocumentSignatureSettingsHandle* settings =
        capsule_cast<DocumentSignatureSettingsHandle>(capsule, SETTINGS_CAPSULE);
    if (settings == nullptr) {
        return nullptr;
    }

    SigningKeyHandle* signing_key =
        capsule_cast<SigningKeyHandle>(signing_key_capsule, "VanillaPDF.SigningKey");
    if (signing_key == nullptr) {
        return nullptr;
    }

    error_type err = DocumentSignatureSettings_SetSigningKey(settings, signing_key);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DocumentSignatureSettings_SetSigningKey");
    }

    Py_RETURN_NONE;
}

PyObject* document_signature_settings_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, SETTINGS_CAPSULE);
}
