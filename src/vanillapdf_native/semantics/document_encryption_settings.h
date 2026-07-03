#ifndef VANILLAPDF_PY_SEMANTICS_DOCUMENT_ENCRYPTION_SETTINGS_H
#define VANILLAPDF_PY_SEMANTICS_DOCUMENT_ENCRYPTION_SETTINGS_H

#include <Python.h>

/* DocumentEncryptionSettings ("VanillaPDF.DocumentEncryptionSettings" capsule).
 * Groups the algorithm, key length, permissions, and passwords that drive
 * Document_AddEncryption. Passwords are write-only (Buffer-typed in the C API). */
PyObject* document_encryption_settings_create(PyObject* self, PyObject* args);
PyObject* document_encryption_settings_get_algorithm(PyObject* self, PyObject* args);
PyObject* document_encryption_settings_set_algorithm(PyObject* self, PyObject* args);
PyObject* document_encryption_settings_get_key_length(PyObject* self, PyObject* args);
PyObject* document_encryption_settings_set_key_length(PyObject* self, PyObject* args);
PyObject* document_encryption_settings_get_user_access_permissions(PyObject* self, PyObject* args);
PyObject* document_encryption_settings_set_user_access_permissions(PyObject* self, PyObject* args);
PyObject* document_encryption_settings_set_user_password(PyObject* self, PyObject* args);
PyObject* document_encryption_settings_set_owner_password(PyObject* self, PyObject* args);
PyObject* document_encryption_settings_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SEMANTICS_DOCUMENT_ENCRYPTION_SETTINGS_H */
