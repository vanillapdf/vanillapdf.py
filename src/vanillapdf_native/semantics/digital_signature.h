#ifndef VANILLAPDF_PY_SEMANTICS_DIGITAL_SIGNATURE_H
#define VANILLAPDF_PY_SEMANTICS_DIGITAL_SIGNATURE_H

#include <Python.h>
#include <vanillapdf/semantics/c_digital_signature.h>

/* Wrap a DigitalSignatureHandle in a capsule (takes ownership). Public so the
 * fields module can return the signature held by a signature field. */
PyObject* digital_signature_capsule_from_handle(DigitalSignatureHandle* handle);

PyObject* digital_signature_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SEMANTICS_DIGITAL_SIGNATURE_H */
