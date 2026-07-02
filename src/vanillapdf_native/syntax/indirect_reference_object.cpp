#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/syntax/c_indirect_reference_object.h>

#include "syntax/indirect_reference_object.h"
#include "syntax/object.h"
#include "common.h"

PyObject* indirect_reference_object_create(PyObject* self, PyObject* args) {
    IndirectReferenceObjectHandle* reference = nullptr;
    error_type create_err = IndirectReferenceObject_Create(&reference);
    if (create_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(create_err, "IndirectReferenceObject_Create");
    }
    auto guard = make_scope_guard([&] { IndirectReferenceObject_Release(reference); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = IndirectReferenceObject_ToObject(reference, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "IndirectReferenceObject_ToObject");
    }

    return object_capsule_from_handle(object);
}

static IndirectReferenceObjectHandle* reference_from_capsule(PyObject* capsule) {
    return object_cast<IndirectReferenceObjectHandle>(
        capsule, IndirectReferenceObject_FromObject, "IndirectReferenceObject_FromObject");
}

PyObject* indirect_reference_object_get_referenced_object_number(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    IndirectReferenceObjectHandle* reference = reference_from_capsule(capsule);
    if (reference == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { IndirectReferenceObject_Release(reference); });

    biguint_type number = 0;
    error_type err = IndirectReferenceObject_GetReferencedObjectNumber(reference, &number);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "IndirectReferenceObject_GetReferencedObjectNumber");
    }

    return to_python(number);
}

PyObject* indirect_reference_object_get_referenced_generation_number(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    IndirectReferenceObjectHandle* reference = reference_from_capsule(capsule);
    if (reference == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { IndirectReferenceObject_Release(reference); });

    ushort_type generation = 0;
    error_type err = IndirectReferenceObject_GetReferencedGenerationNumber(reference, &generation);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "IndirectReferenceObject_GetReferencedGenerationNumber");
    }

    return to_python(generation);
}

PyObject* indirect_reference_object_get_referenced_object(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    IndirectReferenceObjectHandle* reference = reference_from_capsule(capsule);
    if (reference == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { IndirectReferenceObject_Release(reference); });

    ObjectHandle* object = nullptr;
    error_type err = IndirectReferenceObject_GetReferencedObject(reference, &object);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "IndirectReferenceObject_GetReferencedObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* indirect_reference_object_set_object_number(PyObject* self, PyObject* args) {
    PyObject* capsule;
    unsigned long long number;
    if (!PyArg_ParseTuple(args, "OK", &capsule, &number)) {
        return nullptr;
    }

    IndirectReferenceObjectHandle* reference = reference_from_capsule(capsule);
    if (reference == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { IndirectReferenceObject_Release(reference); });

    error_type err = IndirectReferenceObject_SetObjectNumber(reference, number);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "IndirectReferenceObject_SetObjectNumber");
    }

    Py_RETURN_NONE;
}

PyObject* indirect_reference_object_set_generation_number(PyObject* self, PyObject* args) {
    PyObject* capsule;
    int generation_value;
    if (!PyArg_ParseTuple(args, "Oi", &capsule, &generation_value)) {
        return nullptr;
    }

    ushort_type generation = 0;
    if (safe_convert(generation_value, &generation) < 0) {
        return nullptr;
    }

    IndirectReferenceObjectHandle* reference = reference_from_capsule(capsule);
    if (reference == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { IndirectReferenceObject_Release(reference); });

    error_type err = IndirectReferenceObject_SetGenerationNumber(reference, generation);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "IndirectReferenceObject_SetGenerationNumber");
    }

    Py_RETURN_NONE;
}

PyObject* indirect_reference_object_set_referenced_object(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* value_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &value_capsule)) {
        return nullptr;
    }

    ObjectHandle* value = capsule_cast<ObjectHandle>(value_capsule, OBJECT_CAPSULE);
    if (value == nullptr) {
        return nullptr;
    }

    IndirectReferenceObjectHandle* reference = reference_from_capsule(capsule);
    if (reference == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { IndirectReferenceObject_Release(reference); });

    error_type err = IndirectReferenceObject_SetReferencedObject(reference, value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "IndirectReferenceObject_SetReferencedObject");
    }

    Py_RETURN_NONE;
}
