#ifndef VANILLAPDF_PY_UTILS_SIGNATURE_VERIFIER_H
#define VANILLAPDF_PY_UTILS_SIGNATURE_VERIFIER_H

#include <Python.h>

/* TrustedCertificateStore ("VanillaPDF.TrustedCertificateStore") */
PyObject* trusted_certificate_store_create(PyObject* self, PyObject* args);
PyObject* trusted_certificate_store_add_certificate_from_pem(PyObject* self, PyObject* args);
PyObject* trusted_certificate_store_add_certificate_from_der(PyObject* self, PyObject* args);
PyObject* trusted_certificate_store_load_from_directory(PyObject* self, PyObject* args);
PyObject* trusted_certificate_store_load_system_defaults(PyObject* self, PyObject* args);
PyObject* trusted_certificate_store_release(PyObject* self, PyObject* args);

/* SignatureVerificationSettings ("VanillaPDF.SignatureVerificationSettings") */
PyObject* signature_verification_settings_create(PyObject* self, PyObject* args);
PyObject* signature_verification_settings_set_skip_certificate_validation(PyObject* self, PyObject* args);
PyObject* signature_verification_settings_set_allow_weak_algorithms(PyObject* self, PyObject* args);
PyObject* signature_verification_settings_set_check_signing_time(PyObject* self, PyObject* args);
PyObject* signature_verification_settings_release(PyObject* self, PyObject* args);

/* Verify a DigitalSignature -> SignatureVerificationResult
 * ("VanillaPDF.SignatureVerificationResult") */
PyObject* digital_signature_verify(PyObject* self, PyObject* args);
PyObject* signature_verification_result_get_status(PyObject* self, PyObject* args);
PyObject* signature_verification_result_is_signature_valid(PyObject* self, PyObject* args);
PyObject* signature_verification_result_is_document_intact(PyObject* self, PyObject* args);
PyObject* signature_verification_result_is_certificate_trusted(PyObject* self, PyObject* args);
PyObject* signature_verification_result_get_signer_common_name(PyObject* self, PyObject* args);
PyObject* signature_verification_result_get_message(PyObject* self, PyObject* args);
PyObject* signature_verification_result_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_UTILS_SIGNATURE_VERIFIER_H */
