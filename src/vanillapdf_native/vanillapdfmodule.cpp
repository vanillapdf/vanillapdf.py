#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "common.h"
#include "semantics/document.h"
#include "utils/buffer.h"
#include "syntax/file.h"
#include "utils/library_info.h"
#include "utils/logging.h"
#include "utils/errors.h"
#include "utils/license_info.h"
#include "utils/misc_utils.h"
#include "syntax/object.h"
#include "syntax/null_object.h"
#include "syntax/boolean_object.h"
#include "syntax/integer_object.h"
#include "syntax/real_object.h"
#include "syntax/name_object.h"
#include "syntax/string_object.h"

static PyMethodDef VanillapdfMethods[] = {
    /* Document methods */
    {"document_open", document_open, METH_VARARGS, "Open a Document"},
    {"document_save", document_save, METH_VARARGS, "Save a Document"},
    {"document_release", document_release, METH_VARARGS, "Release a Document"},
    /* Buffer methods */
    {"buffer_create", buffer_create, METH_NOARGS, "Create a Buffer"},
    {"buffer_create_from_data", buffer_create_from_data, METH_VARARGS, "Create a Buffer from bytes"},
    {"buffer_set_data", buffer_set_data, METH_VARARGS, "Set Buffer contents"},
    {"buffer_get_data", buffer_get_data, METH_VARARGS, "Get Buffer contents"},
    {"buffer_release", buffer_release, METH_VARARGS, "Release a Buffer"},
    /* File methods */
    {"file_open", file_open, METH_VARARGS, "Open a PDF file"},
    {"file_initialize", file_initialize, METH_VARARGS, "Initialize file (read xref tables)"},
    {"file_get_version", file_get_version, METH_VARARGS, "Get PDF version"},
    {"file_get_filename", file_get_filename, METH_VARARGS, "Get filename"},
    {"file_is_encrypted", file_is_encrypted, METH_VARARGS, "Check if file is encrypted"},
    {"file_set_encryption_password", file_set_encryption_password, METH_VARARGS, "Set encryption password"},
    {"file_release", file_release, METH_VARARGS, "Release a File"},
    /* LibraryInfo methods */
    {"library_info_get_version_major", library_info_get_version_major, METH_NOARGS, "Get library version major"},
    {"library_info_get_version_minor", library_info_get_version_minor, METH_NOARGS, "Get library version minor"},
    {"library_info_get_version_patch", library_info_get_version_patch, METH_NOARGS, "Get library version patch"},
    {"library_info_get_version_build", library_info_get_version_build, METH_NOARGS, "Get library version build"},
    {"library_info_get_author", library_info_get_author, METH_NOARGS, "Get library author"},
    {"library_info_get_build_day", library_info_get_build_day, METH_NOARGS, "Get library build day"},
    {"library_info_get_build_month", library_info_get_build_month, METH_NOARGS, "Get library build month"},
    {"library_info_get_build_year", library_info_get_build_year, METH_NOARGS, "Get library build year"},
    /* Logging methods */
    {"logging_get_severity", logging_get_severity, METH_NOARGS, "Get logging severity"},
    {"logging_set_severity", logging_set_severity, METH_VARARGS, "Set logging severity"},
    {"logging_set_pattern", logging_set_pattern, METH_VARARGS, "Set logging pattern"},
    {"logging_shutdown", logging_shutdown, METH_NOARGS, "Shutdown logging"},
    /* Errors methods */
    {"errors_get_last_error", errors_get_last_error, METH_NOARGS, "Get last error code"},
    {"errors_get_printable_error_text", errors_get_printable_error_text, METH_VARARGS, "Get printable name for an error code"},
    {"errors_get_last_error_message", errors_get_last_error_message, METH_NOARGS, "Get last error detail message"},
    /* LicenseInfo methods */
    {"license_info_set_license_file", license_info_set_license_file, METH_VARARGS, "Set license from a file"},
    {"license_info_set_license_buffer", license_info_set_license_buffer, METH_VARARGS, "Set license from a buffer"},
    {"license_info_is_valid", license_info_is_valid, METH_NOARGS, "Check whether a valid license is set"},
    {"license_info_is_temporary", license_info_is_temporary, METH_NOARGS, "Check whether the license is temporary"},
    /* MiscUtils methods */
    {"misc_utils_initialize_openssl", misc_utils_initialize_openssl, METH_NOARGS, "Initialize OpenSSL"},
    {"misc_utils_cleanup_openssl", misc_utils_cleanup_openssl, METH_NOARGS, "Cleanup OpenSSL"},
    /* Object (base) methods */
    {"object_get_object_type", object_get_object_type, METH_VARARGS, "Get object type"},
    {"object_type_name", object_type_name, METH_VARARGS, "Get printable object type name"},
    {"object_get_offset", object_get_offset, METH_VARARGS, "Get object file offset"},
    {"object_to_string", object_to_string, METH_VARARGS, "Get object string representation"},
    {"object_to_pdf", object_to_pdf, METH_VARARGS, "Get object PDF representation"},
    {"object_release", object_release, METH_VARARGS, "Release an Object"},
    /* Null object */
    {"null_object_create", null_object_create, METH_NOARGS, "Create a Null object"},
    /* Boolean object */
    {"boolean_object_create", boolean_object_create, METH_VARARGS, "Create a Boolean object"},
    {"boolean_object_get_value", boolean_object_get_value, METH_VARARGS, "Get Boolean value"},
    {"boolean_object_set_value", boolean_object_set_value, METH_VARARGS, "Set Boolean value"},
    /* Integer object */
    {"integer_object_create", integer_object_create, METH_VARARGS, "Create an Integer object"},
    {"integer_object_get_value", integer_object_get_value, METH_VARARGS, "Get Integer value"},
    {"integer_object_set_value", integer_object_set_value, METH_VARARGS, "Set Integer value"},
    /* Real object */
    {"real_object_create", real_object_create, METH_VARARGS, "Create a Real object"},
    {"real_object_get_value", real_object_get_value, METH_VARARGS, "Get Real value"},
    {"real_object_set_value", real_object_set_value, METH_VARARGS, "Set Real value"},
    /* Name object */
    {"name_object_create_from_decoded_string", name_object_create_from_decoded_string, METH_VARARGS, "Create a Name object from a decoded string"},
    {"name_object_get_value", name_object_get_value, METH_VARARGS, "Get Name value"},
    /* String object */
    {"string_object_get_string_type", string_object_get_string_type, METH_VARARGS, "Get String subtype"},
    {"string_object_get_value", string_object_get_value, METH_VARARGS, "Get String value"},
    {"string_object_set_value", string_object_set_value, METH_VARARGS, "Set String value"},
    {"literal_string_object_create_from_decoded_string", literal_string_object_create_from_decoded_string, METH_VARARGS, "Create a Literal String object from a decoded string"},
    {"hexadecimal_string_object_create_from_decoded_string", hexadecimal_string_object_create_from_decoded_string, METH_VARARGS, "Create a Hexadecimal String object from a decoded string"},
    {nullptr, nullptr, 0, nullptr}
};

static struct PyModuleDef vanillapdfmodule = {
    PyModuleDef_HEAD_INIT,
    "_vanillapdf",
    "VanillaPDF Python bindings",
    -1,
    VanillapdfMethods
};

PyMODINIT_FUNC
PyInit__vanillapdf(void) {
    PyObject* module = PyModule_Create(&vanillapdfmodule);
    if (module == nullptr) {
        return nullptr;
    }

    if (register_exceptions(module) < 0) {
        Py_DECREF(module);
        return nullptr;
    }

    return module;
}
