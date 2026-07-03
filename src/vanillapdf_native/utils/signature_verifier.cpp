#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/c_handles.h>
#include <vanillapdf/utils/c_buffer.h>
#include <vanillapdf/utils/c_signature_verifier.h>
#include <vanillapdf/utils/c_signature_verification_settings.h>
#include <vanillapdf/semantics/c_digital_signature.h>
#include <vanillapdf/semantics/c_digital_signature_extensions.h>

#include "utils/signature_verifier.h"
#include "utils/buffer.h"
#include "common.h"

static const char* const TRUST_STORE_CAPSULE = "VanillaPDF.TrustedCertificateStore";
static const char* const SETTINGS_CAPSULE = "VanillaPDF.SignatureVerificationSettings";
static const char* const RESULT_CAPSULE = "VanillaPDF.SignatureVerificationResult";

/* ----- TrustedCertificateStore ----- */

PyObject* trusted_certificate_store_create(PyObject* self, PyObject* args) {
    TrustedCertificateStoreHandle* store = nullptr;
    error_type err = TrustedCertificateStore_Create(&store);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "TrustedCertificateStore_Create");
    }

    return capsule_new(store, TRUST_STORE_CAPSULE, RELEASE_FN(TrustedCertificateStore_Release));
}

/* Shared body for the two "add certificate from a Buffer" variants. */
static PyObject* add_certificate(
        PyObject* args,
        error_type (CALLING_CONVENTION *adder)(TrustedCertificateStoreHandle*, const BufferHandle*),
        const char* operation) {
    PyObject* capsule;
    PyObject* buffer_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &buffer_capsule)) {
        return nullptr;
    }

    TrustedCertificateStoreHandle* store =
        capsule_cast<TrustedCertificateStoreHandle>(capsule, TRUST_STORE_CAPSULE);
    if (store == nullptr) {
        return nullptr;
    }

    BufferHandle* buffer = capsule_cast<BufferHandle>(buffer_capsule, "VanillaPDF.Buffer");
    if (buffer == nullptr) {
        return nullptr;
    }

    error_type err = adder(store, buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, operation);
    }

    Py_RETURN_NONE;
}

PyObject* trusted_certificate_store_add_certificate_from_pem(PyObject* self, PyObject* args) {
    return add_certificate(args, TrustedCertificateStore_AddCertificateFromPEM,
                           "TrustedCertificateStore_AddCertificateFromPEM");
}

PyObject* trusted_certificate_store_add_certificate_from_der(PyObject* self, PyObject* args) {
    return add_certificate(args, TrustedCertificateStore_AddCertificateFromDER,
                           "TrustedCertificateStore_AddCertificateFromDER");
}

PyObject* trusted_certificate_store_load_from_directory(PyObject* self, PyObject* args) {
    PyObject* capsule;
    const char* directory;
    if (!PyArg_ParseTuple(args, "Os", &capsule, &directory)) {
        return nullptr;
    }

    TrustedCertificateStoreHandle* store =
        capsule_cast<TrustedCertificateStoreHandle>(capsule, TRUST_STORE_CAPSULE);
    if (store == nullptr) {
        return nullptr;
    }

    error_type err = TrustedCertificateStore_LoadFromDirectory(store, directory);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "TrustedCertificateStore_LoadFromDirectory");
    }

    Py_RETURN_NONE;
}

PyObject* trusted_certificate_store_load_system_defaults(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    TrustedCertificateStoreHandle* store =
        capsule_cast<TrustedCertificateStoreHandle>(capsule, TRUST_STORE_CAPSULE);
    if (store == nullptr) {
        return nullptr;
    }

    error_type err = TrustedCertificateStore_LoadSystemDefaults(store);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "TrustedCertificateStore_LoadSystemDefaults");
    }

    Py_RETURN_NONE;
}

PyObject* trusted_certificate_store_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, TRUST_STORE_CAPSULE);
}

/* ----- SignatureVerificationSettings ----- */

PyObject* signature_verification_settings_create(PyObject* self, PyObject* args) {
    SignatureVerificationSettingsHandle* settings = nullptr;
    error_type err = SignatureVerificationSettings_Create(&settings);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "SignatureVerificationSettings_Create");
    }

    return capsule_new(settings, SETTINGS_CAPSULE, RELEASE_FN(SignatureVerificationSettings_Release));
}

/* Shared body for the boolean flag setters. */
static PyObject* set_flag(
        PyObject* args,
        error_type (CALLING_CONVENTION *setter)(SignatureVerificationSettingsHandle*, boolean_type),
        const char* operation) {
    PyObject* capsule;
    int flag;
    if (!PyArg_ParseTuple(args, "Op", &capsule, &flag)) {
        return nullptr;
    }

    SignatureVerificationSettingsHandle* settings =
        capsule_cast<SignatureVerificationSettingsHandle>(capsule, SETTINGS_CAPSULE);
    if (settings == nullptr) {
        return nullptr;
    }

    boolean_type value = flag ? VANILLAPDF_RV_TRUE : VANILLAPDF_RV_FALSE;
    error_type err = setter(settings, value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, operation);
    }

    Py_RETURN_NONE;
}

PyObject* signature_verification_settings_set_skip_certificate_validation(PyObject* self, PyObject* args) {
    return set_flag(args, SignatureVerificationSettings_SetSkipCertificateValidation,
                    "SignatureVerificationSettings_SetSkipCertificateValidation");
}

PyObject* signature_verification_settings_set_allow_weak_algorithms(PyObject* self, PyObject* args) {
    return set_flag(args, SignatureVerificationSettings_SetAllowWeakAlgorithmsFlag,
                    "SignatureVerificationSettings_SetAllowWeakAlgorithmsFlag");
}

PyObject* signature_verification_settings_set_check_signing_time(PyObject* self, PyObject* args) {
    return set_flag(args, SignatureVerificationSettings_SetCheckSigningTimeFlag,
                    "SignatureVerificationSettings_SetCheckSigningTimeFlag");
}

PyObject* signature_verification_settings_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, SETTINGS_CAPSULE);
}

/* ----- Verify + SignatureVerificationResult ----- */

/* Cast a capsule argument that may be Python None (-> nullptr). Returns false
 * and leaves a set exception only when a non-None value fails the cast. */
template <typename H>
static bool optional_handle(PyObject* obj, const char* name, H** out) {
    if (obj == Py_None) {
        *out = nullptr;
        return true;
    }
    *out = capsule_cast<H>(obj, name);
    return *out != nullptr;
}

PyObject* digital_signature_verify(PyObject* self, PyObject* args) {
    PyObject* signature_capsule;
    PyObject* document_capsule;
    PyObject* trust_store_obj;
    PyObject* settings_obj;
    if (!PyArg_ParseTuple(args, "OOOO", &signature_capsule, &document_capsule,
                          &trust_store_obj, &settings_obj)) {
        return nullptr;
    }

    DigitalSignatureHandle* signature =
        capsule_cast<DigitalSignatureHandle>(signature_capsule, "VanillaPDF.DigitalSignature");
    if (signature == nullptr) {
        return nullptr;
    }

    DocumentHandle* document =
        capsule_cast<DocumentHandle>(document_capsule, "VanillaPDF.Document");
    if (document == nullptr) {
        return nullptr;
    }

    TrustedCertificateStoreHandle* trust_store = nullptr;
    if (!optional_handle(trust_store_obj, TRUST_STORE_CAPSULE, &trust_store)) {
        return nullptr;
    }

    SignatureVerificationSettingsHandle* settings = nullptr;
    if (!optional_handle(settings_obj, SETTINGS_CAPSULE, &settings)) {
        return nullptr;
    }

    SignatureVerificationResultHandle* result = nullptr;
    error_type err = DigitalSignatureExtensions_Verify(
        signature, document, trust_store, settings, &result);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DigitalSignatureExtensions_Verify");
    }

    return capsule_new(result, RESULT_CAPSULE, RELEASE_FN(SignatureVerificationResult_Release));
}

PyObject* signature_verification_result_get_status(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    SignatureVerificationResultHandle* result =
        capsule_cast<SignatureVerificationResultHandle>(capsule, RESULT_CAPSULE);
    if (result == nullptr) {
        return nullptr;
    }

    SignatureVerificationStatusType status = SignatureStatus_Undefined;
    error_type err = SignatureVerificationResult_GetStatus(result, &status);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "SignatureVerificationResult_GetStatus");
    }

    return to_python(status);
}

/* Shared body for the boolean result accessors. */
static PyObject* result_boolean(
        PyObject* args,
        error_type (CALLING_CONVENTION *getter)(SignatureVerificationResultHandle*, boolean_type*),
        const char* operation) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    SignatureVerificationResultHandle* result =
        capsule_cast<SignatureVerificationResultHandle>(capsule, RESULT_CAPSULE);
    if (result == nullptr) {
        return nullptr;
    }

    boolean_type value = VANILLAPDF_RV_FALSE;
    error_type err = getter(result, &value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, operation);
    }

    if (value == VANILLAPDF_RV_TRUE) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

PyObject* signature_verification_result_is_signature_valid(PyObject* self, PyObject* args) {
    return result_boolean(args, SignatureVerificationResult_IsSignatureValid,
                          "SignatureVerificationResult_IsSignatureValid");
}

PyObject* signature_verification_result_is_document_intact(PyObject* self, PyObject* args) {
    return result_boolean(args, SignatureVerificationResult_IsDocumentIntact,
                          "SignatureVerificationResult_IsDocumentIntact");
}

PyObject* signature_verification_result_is_certificate_trusted(PyObject* self, PyObject* args) {
    return result_boolean(args, SignatureVerificationResult_IsCertificateTrusted,
                          "SignatureVerificationResult_IsCertificateTrusted");
}

/* Shared body for the Buffer-returning result accessors (signer name, message).
 * A null out-handle is returned to Python as None. */
static PyObject* result_buffer(
        PyObject* args,
        error_type (CALLING_CONVENTION *getter)(SignatureVerificationResultHandle*, BufferHandle**),
        const char* operation) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    SignatureVerificationResultHandle* result =
        capsule_cast<SignatureVerificationResultHandle>(capsule, RESULT_CAPSULE);
    if (result == nullptr) {
        return nullptr;
    }

    BufferHandle* buffer = nullptr;
    error_type err = getter(result, &buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, operation);
    }

    if (buffer == nullptr) {
        Py_RETURN_NONE;
    }

    return buffer_capsule_from_handle(buffer);
}

PyObject* signature_verification_result_get_signer_common_name(PyObject* self, PyObject* args) {
    return result_buffer(args, SignatureVerificationResult_GetSignerCommonName,
                         "SignatureVerificationResult_GetSignerCommonName");
}

PyObject* signature_verification_result_get_message(PyObject* self, PyObject* args) {
    return result_buffer(args, SignatureVerificationResult_GetMessage,
                         "SignatureVerificationResult_GetMessage");
}

PyObject* signature_verification_result_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, RESULT_CAPSULE);
}
