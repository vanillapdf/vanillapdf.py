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
#include "utils/signing_key.h"
#include "utils/pkcs12_key.h"
#include "syntax/object.h"
#include "syntax/null_object.h"
#include "syntax/boolean_object.h"
#include "syntax/integer_object.h"
#include "syntax/real_object.h"
#include "syntax/name_object.h"
#include "syntax/string_object.h"
#include "syntax/array_object.h"
#include "syntax/dictionary_object.h"
#include "syntax/indirect_reference_object.h"
#include "syntax/stream_object.h"
#include "semantics/catalog.h"
#include "semantics/page_tree.h"
#include "semantics/page_object.h"
#include "semantics/rectangle.h"
#include "semantics/document_info.h"
#include "semantics/document_encryption_settings.h"
#include "semantics/document_signature_settings.h"

static PyMethodDef VanillapdfMethods[] = {
    /* Document methods */
    {"document_open", document_open, METH_VARARGS, "Open a Document"},
    {"document_create", document_create, METH_VARARGS, "Create a new Document"},
    {"document_open_file", document_open_file, METH_VARARGS, "Open a Document from a File"},
    {"document_save", document_save, METH_VARARGS, "Save a Document"},
    {"document_save_incremental", document_save_incremental, METH_VARARGS, "Save a Document incrementally"},
    {"document_get_catalog", document_get_catalog, METH_VARARGS, "Get the Document catalog"},
    {"document_get_document_info", document_get_document_info, METH_VARARGS, "Get the Document info"},
    {"document_append_document", document_append_document, METH_VARARGS, "Append another Document"},
    {"document_add_encryption", document_add_encryption, METH_VARARGS, "Add encryption to a Document"},
    {"document_remove_encryption", document_remove_encryption, METH_VARARGS, "Remove encryption from a Document"},
    {"document_sign", document_sign, METH_VARARGS, "Sign a Document into a destination File"},
    {"document_release", document_release, METH_VARARGS, "Release a Document"},
    /* Buffer methods */
    {"buffer_create", buffer_create, METH_NOARGS, "Create a Buffer"},
    {"buffer_create_from_data", buffer_create_from_data, METH_VARARGS, "Create a Buffer from bytes"},
    {"buffer_set_data", buffer_set_data, METH_VARARGS, "Set Buffer contents"},
    {"buffer_get_data", buffer_get_data, METH_VARARGS, "Get Buffer contents"},
    {"buffer_release", buffer_release, METH_VARARGS, "Release a Buffer"},
    /* File methods */
    {"file_open", file_open, METH_VARARGS, "Open a PDF file"},
    {"file_create", file_create, METH_VARARGS, "Create a new PDF file for writing"},
    {"file_initialize", file_initialize, METH_VARARGS, "Initialize file (read xref tables)"},
    {"file_get_version", file_get_version, METH_VARARGS, "Get PDF version"},
    {"file_get_filename", file_get_filename, METH_VARARGS, "Get filename"},
    {"file_is_encrypted", file_is_encrypted, METH_VARARGS, "Check if file is encrypted"},
    {"file_set_encryption_password", file_set_encryption_password, METH_VARARGS, "Set encryption password"},
    {"file_get_indirect_object", file_get_indirect_object, METH_VARARGS, "Get an indirect object by number/generation"},
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
    /* PKCS12 key + signing key */
    {"pkcs12_key_create_from_file", pkcs12_key_create_from_file, METH_VARARGS, "Load a PKCS#12 key from a file"},
    {"pkcs12_key_create_from_buffer", pkcs12_key_create_from_buffer, METH_VARARGS, "Load a PKCS#12 key from a buffer"},
    {"pkcs12_key_to_signing_key", pkcs12_key_to_signing_key, METH_VARARGS, "Convert a PKCS#12 key to a SigningKey"},
    {"pkcs12_key_release", pkcs12_key_release, METH_VARARGS, "Release a PKCS12Key"},
    {"signing_key_release", signing_key_release, METH_VARARGS, "Release a SigningKey"},
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
    /* Array object */
    {"array_object_create", array_object_create, METH_NOARGS, "Create an Array object"},
    {"array_object_get_size", array_object_get_size, METH_VARARGS, "Get Array size"},
    {"array_object_get_value", array_object_get_value, METH_VARARGS, "Get Array element"},
    {"array_object_set_value", array_object_set_value, METH_VARARGS, "Set Array element"},
    {"array_object_append", array_object_append, METH_VARARGS, "Append to Array"},
    {"array_object_insert", array_object_insert, METH_VARARGS, "Insert into Array"},
    {"array_object_remove", array_object_remove, METH_VARARGS, "Remove Array element"},
    {"array_object_clear", array_object_clear, METH_VARARGS, "Clear Array"},
    /* Dictionary object */
    {"dictionary_object_create", dictionary_object_create, METH_NOARGS, "Create a Dictionary object"},
    {"dictionary_object_get_size", dictionary_object_get_size, METH_VARARGS, "Get Dictionary size"},
    {"dictionary_object_find", dictionary_object_find, METH_VARARGS, "Find value by key"},
    {"dictionary_object_try_find", dictionary_object_try_find, METH_VARARGS, "Find value by key or None"},
    {"dictionary_object_contains", dictionary_object_contains, METH_VARARGS, "Check if key present"},
    {"dictionary_object_get_iterator", dictionary_object_get_iterator, METH_VARARGS, "Get Dictionary iterator"},
    {"dictionary_object_remove", dictionary_object_remove, METH_VARARGS, "Remove key"},
    {"dictionary_object_clear", dictionary_object_clear, METH_VARARGS, "Clear Dictionary"},
    {"dictionary_object_insert", dictionary_object_insert, METH_VARARGS, "Insert key/value"},
    {"dictionary_iterator_get_key", dictionary_iterator_get_key, METH_VARARGS, "Iterator current key"},
    {"dictionary_iterator_get_value", dictionary_iterator_get_value, METH_VARARGS, "Iterator current value"},
    {"dictionary_iterator_is_valid", dictionary_iterator_is_valid, METH_VARARGS, "Iterator is valid"},
    {"dictionary_iterator_next", dictionary_iterator_next, METH_VARARGS, "Advance iterator"},
    {"dictionary_iterator_release", dictionary_iterator_release, METH_VARARGS, "Release iterator"},
    /* Indirect reference object */
    {"indirect_reference_object_create", indirect_reference_object_create, METH_NOARGS, "Create an Indirect reference"},
    {"indirect_reference_object_get_referenced_object_number", indirect_reference_object_get_referenced_object_number, METH_VARARGS, "Get referenced object number"},
    {"indirect_reference_object_get_referenced_generation_number", indirect_reference_object_get_referenced_generation_number, METH_VARARGS, "Get referenced generation number"},
    {"indirect_reference_object_get_referenced_object", indirect_reference_object_get_referenced_object, METH_VARARGS, "Get referenced object"},
    {"indirect_reference_object_set_object_number", indirect_reference_object_set_object_number, METH_VARARGS, "Set referenced object number"},
    {"indirect_reference_object_set_generation_number", indirect_reference_object_set_generation_number, METH_VARARGS, "Set referenced generation number"},
    {"indirect_reference_object_set_referenced_object", indirect_reference_object_set_referenced_object, METH_VARARGS, "Set referenced object"},
    /* Stream object */
    {"stream_object_create", stream_object_create, METH_NOARGS, "Create a Stream object"},
    {"stream_object_get_header", stream_object_get_header, METH_VARARGS, "Get Stream header dictionary"},
    {"stream_object_set_header", stream_object_set_header, METH_VARARGS, "Set Stream header dictionary"},
    {"stream_object_get_body", stream_object_get_body, METH_VARARGS, "Get Stream body (decoded)"},
    {"stream_object_get_body_raw", stream_object_get_body_raw, METH_VARARGS, "Get Stream body (raw)"},
    {"stream_object_set_body", stream_object_set_body, METH_VARARGS, "Set Stream body"},
    /* Catalog */
    {"catalog_get_pages", catalog_get_pages, METH_VARARGS, "Get the page tree"},
    {"catalog_release", catalog_release, METH_VARARGS, "Release a Catalog"},
    /* Page tree */
    {"page_tree_get_page_count", page_tree_get_page_count, METH_VARARGS, "Get page count"},
    {"page_tree_get_page", page_tree_get_page, METH_VARARGS, "Get a page by index"},
    {"page_tree_insert_page", page_tree_insert_page, METH_VARARGS, "Insert a page"},
    {"page_tree_append_page", page_tree_append_page, METH_VARARGS, "Append a page"},
    {"page_tree_remove_page", page_tree_remove_page, METH_VARARGS, "Remove a page"},
    {"page_tree_release", page_tree_release, METH_VARARGS, "Release a PageTree"},
    /* Page object */
    {"page_object_create_from_document", page_object_create_from_document, METH_VARARGS, "Create a page for a document"},
    {"page_object_get_media_box", page_object_get_media_box, METH_VARARGS, "Get the media box"},
    {"page_object_set_media_box", page_object_set_media_box, METH_VARARGS, "Set the media box"},
    {"page_object_get_base_object", page_object_get_base_object, METH_VARARGS, "Get the underlying dictionary"},
    {"page_object_release", page_object_release, METH_VARARGS, "Release a PageObject"},
    /* Rectangle */
    {"rectangle_create", rectangle_create, METH_NOARGS, "Create a Rectangle"},
    {"rectangle_get_lower_left_x", rectangle_get_lower_left_x, METH_VARARGS, "Get lower-left X"},
    {"rectangle_set_lower_left_x", rectangle_set_lower_left_x, METH_VARARGS, "Set lower-left X"},
    {"rectangle_get_lower_left_y", rectangle_get_lower_left_y, METH_VARARGS, "Get lower-left Y"},
    {"rectangle_set_lower_left_y", rectangle_set_lower_left_y, METH_VARARGS, "Set lower-left Y"},
    {"rectangle_get_upper_right_x", rectangle_get_upper_right_x, METH_VARARGS, "Get upper-right X"},
    {"rectangle_set_upper_right_x", rectangle_set_upper_right_x, METH_VARARGS, "Set upper-right X"},
    {"rectangle_get_upper_right_y", rectangle_get_upper_right_y, METH_VARARGS, "Get upper-right Y"},
    {"rectangle_set_upper_right_y", rectangle_set_upper_right_y, METH_VARARGS, "Set upper-right Y"},
    {"rectangle_release", rectangle_release, METH_VARARGS, "Release a Rectangle"},
    /* Document info */
    {"document_info_get_title", document_info_get_title, METH_VARARGS, "Get title"},
    {"document_info_get_author", document_info_get_author, METH_VARARGS, "Get author"},
    {"document_info_get_subject", document_info_get_subject, METH_VARARGS, "Get subject"},
    {"document_info_get_keywords", document_info_get_keywords, METH_VARARGS, "Get keywords"},
    {"document_info_get_creator", document_info_get_creator, METH_VARARGS, "Get creator"},
    {"document_info_get_producer", document_info_get_producer, METH_VARARGS, "Get producer"},
    {"document_info_release", document_info_release, METH_VARARGS, "Release a DocumentInfo"},
    /* Document encryption settings */
    {"document_encryption_settings_create", document_encryption_settings_create, METH_NOARGS, "Create DocumentEncryptionSettings"},
    {"document_encryption_settings_get_algorithm", document_encryption_settings_get_algorithm, METH_VARARGS, "Get encryption algorithm"},
    {"document_encryption_settings_set_algorithm", document_encryption_settings_set_algorithm, METH_VARARGS, "Set encryption algorithm"},
    {"document_encryption_settings_get_key_length", document_encryption_settings_get_key_length, METH_VARARGS, "Get encryption key length"},
    {"document_encryption_settings_set_key_length", document_encryption_settings_set_key_length, METH_VARARGS, "Set encryption key length"},
    {"document_encryption_settings_get_user_access_permissions", document_encryption_settings_get_user_access_permissions, METH_VARARGS, "Get user access permissions"},
    {"document_encryption_settings_set_user_access_permissions", document_encryption_settings_set_user_access_permissions, METH_VARARGS, "Set user access permissions"},
    {"document_encryption_settings_set_user_password", document_encryption_settings_set_user_password, METH_VARARGS, "Set user password"},
    {"document_encryption_settings_set_owner_password", document_encryption_settings_set_owner_password, METH_VARARGS, "Set owner password"},
    {"document_encryption_settings_release", document_encryption_settings_release, METH_VARARGS, "Release DocumentEncryptionSettings"},
    /* Document signature settings */
    {"document_signature_settings_create", document_signature_settings_create, METH_NOARGS, "Create DocumentSignatureSettings"},
    {"document_signature_settings_get_digest", document_signature_settings_get_digest, METH_VARARGS, "Get signature digest algorithm"},
    {"document_signature_settings_set_digest", document_signature_settings_set_digest, METH_VARARGS, "Set signature digest algorithm"},
    {"document_signature_settings_set_signing_key", document_signature_settings_set_signing_key, METH_VARARGS, "Set signing key"},
    {"document_signature_settings_release", document_signature_settings_release, METH_VARARGS, "Release DocumentSignatureSettings"},
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
