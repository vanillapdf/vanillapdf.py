#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/semantics/c_fields.h>
#include <vanillapdf/semantics/c_digital_signature.h>

#include "semantics/fields.h"
#include "semantics/digital_signature.h"
#include "common.h"

static const char* const FIELD_COLLECTION_CAPSULE = "VanillaPDF.FieldCollection";
static const char* const FIELD_CAPSULE = "VanillaPDF.Field";
static const char* const SIGNATURE_FIELD_CAPSULE = "VanillaPDF.SignatureField";

PyObject* field_collection_capsule_from_handle(FieldCollectionHandle* handle) {
    return capsule_new(handle, FIELD_COLLECTION_CAPSULE, RELEASE_FN(FieldCollection_Release));
}

static PyObject* field_capsule_from_handle(FieldHandle* handle) {
    return capsule_new(handle, FIELD_CAPSULE, RELEASE_FN(Field_Release));
}

static PyObject* signature_field_capsule_from_handle(SignatureFieldHandle* handle) {
    return capsule_new(handle, SIGNATURE_FIELD_CAPSULE, RELEASE_FN(SignatureField_Release));
}

PyObject* field_collection_get_size(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    FieldCollectionHandle* fields =
        capsule_cast<FieldCollectionHandle>(capsule, FIELD_COLLECTION_CAPSULE);
    if (fields == nullptr) {
        return nullptr;
    }

    size_type size = 0;
    error_type err = FieldCollection_GetSize(fields, &size);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "FieldCollection_GetSize");
    }

    return to_python(size);
}

PyObject* field_collection_at(PyObject* self, PyObject* args) {
    PyObject* capsule;
    Py_ssize_t index;
    if (!PyArg_ParseTuple(args, "On", &capsule, &index)) {
        return nullptr;
    }

    size_type native_index = 0;
    if (safe_convert(index, &native_index) < 0) {
        return nullptr;
    }

    FieldCollectionHandle* fields =
        capsule_cast<FieldCollectionHandle>(capsule, FIELD_COLLECTION_CAPSULE);
    if (fields == nullptr) {
        return nullptr;
    }

    FieldHandle* field = nullptr;
    error_type err = FieldCollection_At(fields, native_index, &field);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "FieldCollection_At");
    }

    return field_capsule_from_handle(field);
}

PyObject* field_collection_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, FIELD_COLLECTION_CAPSULE);
}

PyObject* field_get_type(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    FieldHandle* field = capsule_cast<FieldHandle>(capsule, FIELD_CAPSULE);
    if (field == nullptr) {
        return nullptr;
    }

    FieldType type = FieldType_Undefined;
    error_type err = Field_GetType(field, &type);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Field_GetType");
    }

    return to_python(type);
}

PyObject* field_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, FIELD_CAPSULE);
}

PyObject* signature_field_from_field(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    FieldHandle* field = capsule_cast<FieldHandle>(capsule, FIELD_CAPSULE);
    if (field == nullptr) {
        return nullptr;
    }

    SignatureFieldHandle* signature_field = nullptr;
    error_type err = SignatureField_FromField(field, &signature_field);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "SignatureField_FromField");
    }

    return signature_field_capsule_from_handle(signature_field);
}

PyObject* signature_field_get_value(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    SignatureFieldHandle* signature_field =
        capsule_cast<SignatureFieldHandle>(capsule, SIGNATURE_FIELD_CAPSULE);
    if (signature_field == nullptr) {
        return nullptr;
    }

    DigitalSignatureHandle* signature = nullptr;
    error_type err = SignatureField_GetValue(signature_field, &signature);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "SignatureField_GetValue");
    }

    return digital_signature_capsule_from_handle(signature);
}

PyObject* signature_field_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, SIGNATURE_FIELD_CAPSULE);
}
