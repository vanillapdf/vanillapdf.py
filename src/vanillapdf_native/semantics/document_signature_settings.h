#ifndef VANILLAPDF_PY_SEMANTICS_DOCUMENT_SIGNATURE_SETTINGS_H
#define VANILLAPDF_PY_SEMANTICS_DOCUMENT_SIGNATURE_SETTINGS_H

#include <Python.h>

/* DocumentSignatureSettings ("VanillaPDF.DocumentSignatureSettings" capsule).
 * Holds the signing key and digest algorithm that drive Document_Sign. */
PyObject* document_signature_settings_create(PyObject* self, PyObject* args);
PyObject* document_signature_settings_get_digest(PyObject* self, PyObject* args);
PyObject* document_signature_settings_set_digest(PyObject* self, PyObject* args);
PyObject* document_signature_settings_set_signing_key(PyObject* self, PyObject* args);
PyObject* document_signature_settings_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SEMANTICS_DOCUMENT_SIGNATURE_SETTINGS_H */
