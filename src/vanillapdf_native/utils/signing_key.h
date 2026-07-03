#ifndef VANILLAPDF_PY_UTILS_SIGNING_KEY_H
#define VANILLAPDF_PY_UTILS_SIGNING_KEY_H

#include <Python.h>
#include <vanillapdf/utils/c_signing_key.h>

/* Wrap a SigningKeyHandle in a capsule (takes ownership). Public so the PKCS12
 * module can return a SigningKey produced by PKCS12Key_ToSigningKey. */
PyObject* signing_key_capsule_from_handle(SigningKeyHandle* handle);

PyObject* signing_key_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_UTILS_SIGNING_KEY_H */
