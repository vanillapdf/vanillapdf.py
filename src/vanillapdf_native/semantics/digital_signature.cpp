#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/semantics/c_digital_signature.h>

#include "semantics/digital_signature.h"
#include "common.h"

static const char* const DIGITAL_SIGNATURE_CAPSULE = "VanillaPDF.DigitalSignature";

PyObject* digital_signature_capsule_from_handle(DigitalSignatureHandle* handle) {
    return capsule_new(handle, DIGITAL_SIGNATURE_CAPSULE, RELEASE_FN(DigitalSignature_Release));
}

PyObject* digital_signature_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, DIGITAL_SIGNATURE_CAPSULE);
}
