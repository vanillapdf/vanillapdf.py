#ifndef VANILLAPDF_PY_SYNTAX_OBJECT_H
#define VANILLAPDF_PY_SYNTAX_OBJECT_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/syntax/c_object.h>

/* Base syntax Object. Every typed object (integer, dictionary, ...) is
 * exposed to Python as a "VanillaPDF.Object" capsule; the typed modules
 * convert to their concrete handle internally via X_FromObject. */

/* Capsule name shared by every syntax object exposed to Python. */
extern const char* const OBJECT_CAPSULE;

/* Wrap an ObjectHandle in an object capsule (takes ownership). Used by any
 * function that produces a base Object (container getters, X_ToObject). */
PyObject* object_capsule_from_handle(ObjectHandle* handle);

/* object_get_object_type(capsule) -> int (ObjectType) */
PyObject* object_get_object_type(PyObject* self, PyObject* args);

/* object_type_name(type: int) -> str (static) */
PyObject* object_type_name(PyObject* self, PyObject* args);

/* object_get_offset(capsule) -> int */
PyObject* object_get_offset(PyObject* self, PyObject* args);

/* object_to_string(capsule) -> Buffer */
PyObject* object_to_string(PyObject* self, PyObject* args);

/* object_to_pdf(capsule) -> Buffer */
PyObject* object_to_pdf(PyObject* self, PyObject* args);

/* object_release(capsule) -> None */
PyObject* object_release(PyObject* self, PyObject* args);

#ifdef __cplusplus

#include "common.h"

/* Convert a base Object capsule to a concrete typed handle via `from_object`
 * (e.g. IntegerObject_FromObject). Returns nullptr with a Python exception set
 * on failure; on success the caller owns the returned handle and must release
 * it (typically via a scope guard). This centralizes the capsule_cast +
 * X_FromObject dance every typed accessor performs. */
template <typename H>
H* object_cast(PyObject* capsule,
               error_type (CALLING_CONVENTION *from_object)(ObjectHandle*, H**),
               const char* operation) {
    ObjectHandle* object = capsule_cast<ObjectHandle>(capsule, OBJECT_CAPSULE);
    if (object == nullptr) {
        return nullptr;
    }

    H* typed = nullptr;
    error_type err = from_object(object, &typed);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        raise_last_error(err, operation);
        return nullptr;
    }

    return typed;
}

#endif /* __cplusplus */

#endif /* VANILLAPDF_PY_SYNTAX_OBJECT_H */
