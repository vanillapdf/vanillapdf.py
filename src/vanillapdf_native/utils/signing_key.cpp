#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_signing_key.h>

#include "utils/signing_key.h"
#include "common.h"

static const char* const SIGNING_KEY_CAPSULE = "VanillaPDF.SigningKey";

PyObject* signing_key_capsule_from_handle(SigningKeyHandle* handle) {
    return capsule_new(handle, SIGNING_KEY_CAPSULE, RELEASE_FN(SigningKey_Release));
}

PyObject* signing_key_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, SIGNING_KEY_CAPSULE);
}
