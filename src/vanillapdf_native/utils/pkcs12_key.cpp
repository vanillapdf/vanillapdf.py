#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_buffer.h>
#include <vanillapdf/utils/c_pkcs12_key.h>
#include <vanillapdf/utils/c_signing_key.h>

#include "utils/pkcs12_key.h"
#include "utils/signing_key.h"
#include "common.h"

static const char* const PKCS12_KEY_CAPSULE = "VanillaPDF.PKCS12Key";

static PyObject* pkcs12_key_capsule_from_handle(PKCS12KeyHandle* handle) {
    return capsule_new(handle, PKCS12_KEY_CAPSULE, RELEASE_FN(PKCS12Key_Release));
}

PyObject* pkcs12_key_create_from_file(PyObject* self, PyObject* args) {
    const char* path;
    const char* password;
    if (!PyArg_ParseTuple(args, "ss", &path, &password)) {
        return nullptr;
    }

    PKCS12KeyHandle* key = nullptr;
    error_type err = without_gil([path, password, &key] {
        return PKCS12Key_CreateFromFile(path, password, &key);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "PKCS12Key_CreateFromFile");
    }

    return pkcs12_key_capsule_from_handle(key);
}

PyObject* pkcs12_key_create_from_buffer(PyObject* self, PyObject* args) {
    PyObject* buffer_capsule;
    const char* password;
    if (!PyArg_ParseTuple(args, "Os", &buffer_capsule, &password)) {
        return nullptr;
    }

    BufferHandle* buffer = capsule_cast<BufferHandle>(buffer_capsule, "VanillaPDF.Buffer");
    if (buffer == nullptr) {
        return nullptr;
    }

    PKCS12KeyHandle* key = nullptr;
    error_type err = without_gil([buffer, password, &key] {
        return PKCS12Key_CreateFromBuffer(buffer, password, &key);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "PKCS12Key_CreateFromBuffer");
    }

    return pkcs12_key_capsule_from_handle(key);
}

PyObject* pkcs12_key_to_signing_key(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    PKCS12KeyHandle* key = capsule_cast<PKCS12KeyHandle>(capsule, PKCS12_KEY_CAPSULE);
    if (key == nullptr) {
        return nullptr;
    }

    SigningKeyHandle* signing_key = nullptr;
    error_type err = without_gil([key, &signing_key] {
        return PKCS12Key_ToSigningKey(key, &signing_key);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "PKCS12Key_ToSigningKey");
    }

    return signing_key_capsule_from_handle(signing_key);
}

PyObject* pkcs12_key_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, PKCS12_KEY_CAPSULE);
}
