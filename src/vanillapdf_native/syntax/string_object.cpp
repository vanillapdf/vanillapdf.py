#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_buffer.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/syntax/c_string_object.h>

#include "syntax/string_object.h"
#include "syntax/object.h"
#include "utils/buffer.h"
#include "common.h"

/* ---- String (base) ------------------------------------------------------ */

PyObject* string_object_get_string_type(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    StringObjectHandle* string = object_cast<StringObjectHandle>(
        capsule, StringObject_FromObject, "StringObject_FromObject");
    if (string == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { StringObject_Release(string); });

    StringType type = StringType_Undefined;
    error_type err = StringObject_GetStringType(string, &type);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "StringObject_GetStringType");
    }

    return PyLong_FromLong(static_cast<long>(type));
}

PyObject* string_object_get_value(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    StringObjectHandle* string = object_cast<StringObjectHandle>(
        capsule, StringObject_FromObject, "StringObject_FromObject");
    if (string == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { StringObject_Release(string); });

    BufferHandle* buffer = nullptr;
    error_type err = StringObject_GetValue(string, &buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "StringObject_GetValue");
    }

    return buffer_capsule_from_handle(buffer);
}

PyObject* string_object_set_value(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* buffer_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &buffer_capsule)) {
        return nullptr;
    }

    BufferHandle* buffer = capsule_cast<BufferHandle>(buffer_capsule, "VanillaPDF.Buffer");
    if (buffer == nullptr) {
        return nullptr;
    }

    StringObjectHandle* string = object_cast<StringObjectHandle>(
        capsule, StringObject_FromObject, "StringObject_FromObject");
    if (string == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { StringObject_Release(string); });

    error_type err = StringObject_SetValue(string, buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "StringObject_SetValue");
    }

    Py_RETURN_NONE;
}

/* ---- Literal / Hexadecimal string constructors -------------------------- */

PyObject* literal_string_object_create_from_decoded_string(PyObject* self, PyObject* args) {
    const char* value;
    if (!PyArg_ParseTuple(args, "s", &value)) {
        return nullptr;
    }

    LiteralStringObjectHandle* literal = nullptr;
    error_type create_err = LiteralStringObject_CreateFromDecodedString(value, &literal);
    if (create_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(create_err, "LiteralStringObject_CreateFromDecodedString");
    }
    auto literal_guard = make_scope_guard([&] { LiteralStringObject_Release(literal); });

    StringObjectHandle* string = nullptr;
    error_type to_string_err = LiteralStringObject_ToStringObject(literal, &string);
    if (to_string_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_string_err, "LiteralStringObject_ToStringObject");
    }
    auto string_guard = make_scope_guard([&] { StringObject_Release(string); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = StringObject_ToObject(string, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "StringObject_ToObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* hexadecimal_string_object_create_from_decoded_string(PyObject* self, PyObject* args) {
    const char* value;
    if (!PyArg_ParseTuple(args, "s", &value)) {
        return nullptr;
    }

    HexadecimalStringObjectHandle* hex = nullptr;
    error_type create_err = HexadecimalStringObject_CreateFromDecodedString(value, &hex);
    if (create_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(create_err, "HexadecimalStringObject_CreateFromDecodedString");
    }
    auto hex_guard = make_scope_guard([&] { HexadecimalStringObject_Release(hex); });

    StringObjectHandle* string = nullptr;
    error_type to_string_err = HexadecimalStringObject_ToStringObject(hex, &string);
    if (to_string_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_string_err, "HexadecimalStringObject_ToStringObject");
    }
    auto string_guard = make_scope_guard([&] { StringObject_Release(string); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = StringObject_ToObject(string, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "StringObject_ToObject");
    }

    return object_capsule_from_handle(object);
}
