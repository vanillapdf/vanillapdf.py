#ifndef VANILLAPDF_PY_UTILS_PKCS12_KEY_H
#define VANILLAPDF_PY_UTILS_PKCS12_KEY_H

#include <Python.h>

/* PKCS12Key ("VanillaPDF.PKCS12Key" capsule). A PKCS#12 (.pfx/.p12) key +
 * certificate, convertible to a SigningKey for document signing. */
PyObject* pkcs12_key_create_from_file(PyObject* self, PyObject* args);
PyObject* pkcs12_key_create_from_buffer(PyObject* self, PyObject* args);
PyObject* pkcs12_key_to_signing_key(PyObject* self, PyObject* args);
PyObject* pkcs12_key_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_UTILS_PKCS12_KEY_H */
