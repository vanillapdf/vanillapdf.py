#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/syntax/c_string_object.h>
#include <vanillapdf/semantics/c_document_info.h>

#include "semantics/document_info.h"
#include "syntax/object.h"
#include "common.h"

static const char* const DOCUMENT_INFO_CAPSULE = "VanillaPDF.DocumentInfo";

PyObject* document_info_capsule_from_handle(DocumentInfoHandle* handle) {
    return capsule_new(handle, DOCUMENT_INFO_CAPSULE, RELEASE_FN(DocumentInfo_Release));
}

/* Shared body for the string metadata getters: returns a String Object, or
 * None when the entry is absent (VANILLAPDF_ERROR_*_MISSING). */
static PyObject* get_string_property(
        PyObject* args,
        error_type (CALLING_CONVENTION *getter)(DocumentInfoHandle*, StringObjectHandle**),
        const char* operation) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DocumentInfoHandle* info = capsule_cast<DocumentInfoHandle>(capsule, DOCUMENT_INFO_CAPSULE);
    if (info == nullptr) {
        return nullptr;
    }

    StringObjectHandle* string = nullptr;
    error_type err = getter(info, &string);
    if (is_missing_error(err)) {
        Py_RETURN_NONE;
    }
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, operation);
    }
    auto guard = make_scope_guard([&] { StringObject_Release(string); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = StringObject_ToObject(string, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "StringObject_ToObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* document_info_get_title(PyObject* self, PyObject* args) {
    return get_string_property(args, DocumentInfo_GetTitle, "DocumentInfo_GetTitle");
}

PyObject* document_info_get_author(PyObject* self, PyObject* args) {
    return get_string_property(args, DocumentInfo_GetAuthor, "DocumentInfo_GetAuthor");
}

PyObject* document_info_get_subject(PyObject* self, PyObject* args) {
    return get_string_property(args, DocumentInfo_GetSubject, "DocumentInfo_GetSubject");
}

PyObject* document_info_get_keywords(PyObject* self, PyObject* args) {
    return get_string_property(args, DocumentInfo_GetKeywords, "DocumentInfo_GetKeywords");
}

PyObject* document_info_get_creator(PyObject* self, PyObject* args) {
    return get_string_property(args, DocumentInfo_GetCreator, "DocumentInfo_GetCreator");
}

PyObject* document_info_get_producer(PyObject* self, PyObject* args) {
    return get_string_property(args, DocumentInfo_GetProducer, "DocumentInfo_GetProducer");
}

PyObject* document_info_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, DOCUMENT_INFO_CAPSULE);
}
