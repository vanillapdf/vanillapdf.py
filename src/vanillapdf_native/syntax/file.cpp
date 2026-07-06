#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_file.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/utils/c_pdf_version.h>

#include "syntax/file.h"
#include "syntax/object.h"
#include "utils/buffer.h"
#include "common.h"

static const char* const FILE_CAPSULE = "VanillaPDF.File";

PyObject* file_open(PyObject* self, PyObject* args) {
    const char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return nullptr;
    }

    FileHandle* handle = nullptr;
    error_type err = without_gil([filename, &handle] {
        return File_Open(filename, &handle);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "File_Open");
    }

    return capsule_new(handle, FILE_CAPSULE, RELEASE_FN(File_Release));
}

PyObject* file_create(PyObject* self, PyObject* args) {
    const char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return nullptr;
    }

    FileHandle* handle = nullptr;
    error_type err = without_gil([filename, &handle] {
        return File_Create(filename, &handle);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "File_Create");
    }

    return capsule_new(handle, FILE_CAPSULE, RELEASE_FN(File_Release));
}

PyObject* file_initialize(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    FileHandle* handle = capsule_cast<FileHandle>(capsule, FILE_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    error_type err = without_gil([handle] {
        return File_Initialize(handle);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "File_Initialize");
    }

    Py_RETURN_NONE;
}

PyObject* file_get_version(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    FileHandle* handle = capsule_cast<FileHandle>(capsule, FILE_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    PDFVersion version;
    error_type err = File_GetVersion(handle, &version);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "File_GetVersion");
    }

    return to_python(version);
}

PyObject* file_get_filename(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    FileHandle* handle = capsule_cast<FileHandle>(capsule, FILE_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    BufferHandle* buffer = nullptr;
    error_type err = File_GetFilename(handle, &buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "File_GetFilename");
    }

    return buffer_capsule_from_handle(buffer);
}

PyObject* file_is_encrypted(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    FileHandle* handle = capsule_cast<FileHandle>(capsule, FILE_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    boolean_type is_encrypted = VANILLAPDF_RV_FALSE;
    error_type err = File_IsEncrypted(handle, &is_encrypted);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "File_IsEncrypted");
    }

    if (is_encrypted == VANILLAPDF_RV_TRUE) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

PyObject* file_set_encryption_password(PyObject* self, PyObject* args) {
    PyObject* capsule;
    const char* password;
    if (!PyArg_ParseTuple(args, "Os", &capsule, &password)) {
        return nullptr;
    }

    FileHandle* handle = capsule_cast<FileHandle>(capsule, FILE_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    error_type err = without_gil([handle, password] {
        return File_SetEncryptionPassword(handle, password);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "File_SetEncryptionPassword");
    }

    Py_RETURN_NONE;
}

PyObject* file_get_indirect_object(PyObject* self, PyObject* args) {
    PyObject* capsule;
    unsigned long long object_number;
    int generation_value;
    if (!PyArg_ParseTuple(args, "OKi", &capsule, &object_number, &generation_value)) {
        return nullptr;
    }

    ushort_type generation = 0;
    if (safe_convert(generation_value, &generation) < 0) {
        return nullptr;
    }

    FileHandle* handle = capsule_cast<FileHandle>(capsule, FILE_CAPSULE);
    if (handle == nullptr) {
        return nullptr;
    }

    ObjectHandle* object = nullptr;
    error_type err = without_gil([handle, object_number, generation, &object] {
        return File_GetIndirectObject(handle, object_number, generation, &object);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "File_GetIndirectObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* file_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, FILE_CAPSULE);
}
