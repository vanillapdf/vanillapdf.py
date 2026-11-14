#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_file.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/utils/c_input_output_stream.h>
#include <vanillapdf/utils/c_io_strategy.h>
#include <vanillapdf/utils/c_pdf_version.h>

#include "syntax/file.h"
#include "syntax/object.h"
#include "utils/buffer.h"
#include "common.h"

static const char* const FILE_CAPSULE = "VanillaPDF.File";

PyObject* file_open(PyObject* self, PyObject* args) {
    const char* filename;
    int strategy;
    if (!PyArg_ParseTuple(args, "si", &filename, &strategy)) {
        return nullptr;
    }

    IOStrategyType strategy_value = static_cast<IOStrategyType>(strategy);
    FileHandle* handle = nullptr;
    error_type err = without_gil([filename, strategy_value, &handle] {
        return File_OpenWithStrategy(filename, strategy_value, &handle);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "File_OpenWithStrategy");
    }

    return capsule_new(handle, FILE_CAPSULE, RELEASE_FN(File_Release));
}

PyObject* file_create(PyObject* self, PyObject* args) {
    const char* filename;
    int strategy;
    if (!PyArg_ParseTuple(args, "si", &filename, &strategy)) {
        return nullptr;
    }

    IOStrategyType strategy_value = static_cast<IOStrategyType>(strategy);
    FileHandle* handle = nullptr;
    error_type err = without_gil([filename, strategy_value, &handle] {
        return File_CreateWithStrategy(filename, strategy_value, &handle);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "File_CreateWithStrategy");
    }

    return capsule_new(handle, FILE_CAPSULE, RELEASE_FN(File_Release));
}

PyObject* file_open_stream(PyObject* self, PyObject* args) {
    PyObject* stream_capsule;
    const char* name;
    if (!PyArg_ParseTuple(args, "Os", &stream_capsule, &name)) {
        return nullptr;
    }

    auto stream =
        capsule_cast<InputOutputStreamHandle>(stream_capsule, "VanillaPDF.InputOutputStream");
    if (!stream) {
        return nullptr;
    }

    FileHandle* handle = nullptr;
    error_type err = without_gil([stream = stream.get(), name, &handle] {
        return File_OpenStream(stream, name, &handle);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "File_OpenStream");
    }

    return capsule_new(handle, FILE_CAPSULE, RELEASE_FN(File_Release));
}

PyObject* file_create_stream(PyObject* self, PyObject* args) {
    PyObject* stream_capsule;
    const char* name;
    if (!PyArg_ParseTuple(args, "Os", &stream_capsule, &name)) {
        return nullptr;
    }

    auto stream =
        capsule_cast<InputOutputStreamHandle>(stream_capsule, "VanillaPDF.InputOutputStream");
    if (!stream) {
        return nullptr;
    }

    FileHandle* handle = nullptr;
    error_type err = without_gil([stream = stream.get(), name, &handle] {
        return File_CreateStream(stream, name, &handle);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "File_CreateStream");
    }

    return capsule_new(handle, FILE_CAPSULE, RELEASE_FN(File_Release));
}

PyObject* file_initialize(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    auto handle = capsule_cast<FileHandle>(capsule, FILE_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    error_type err = without_gil([handle = handle.get()] {
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

    auto handle = capsule_cast<FileHandle>(capsule, FILE_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    PDFVersion version;
    error_type err = without_gil([handle = handle.get(), &version] {
        return File_GetVersion(handle, &version);
    });
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

    auto handle = capsule_cast<FileHandle>(capsule, FILE_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    BufferHandle* buffer = nullptr;
    error_type err = without_gil([handle = handle.get(), &buffer] {
        return File_GetFilename(handle, &buffer);
    });
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

    auto handle = capsule_cast<FileHandle>(capsule, FILE_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    boolean_type is_encrypted = VANILLAPDF_RV_FALSE;
    error_type err = without_gil([handle = handle.get(), &is_encrypted] {
        return File_IsEncrypted(handle, &is_encrypted);
    });
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

    auto handle = capsule_cast<FileHandle>(capsule, FILE_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    error_type err = without_gil([handle = handle.get(), password] {
        return File_SetEncryptionPassword(handle, password);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "File_SetEncryptionPassword");
    }

    Py_RETURN_NONE;
}

PyObject* file_get_indirect_object(PyObject* self, PyObject* args) {
    PyObject* capsule;
    long long object_value;
    int generation_value;
    if (!PyArg_ParseTuple(args, "OLi", &capsule, &object_value, &generation_value)) {
        return nullptr;
    }

    /* Parse signed and range-check: the unsigned 'K' format silently wraps
     * negative and oversized values into a different object number. */
    biguint_type object_number = 0;
    if (safe_convert(object_value, &object_number) < 0) {
        return nullptr;
    }

    ushort_type generation = 0;
    if (safe_convert(generation_value, &generation) < 0) {
        return nullptr;
    }

    auto handle = capsule_cast<FileHandle>(capsule, FILE_CAPSULE);
    if (!handle) {
        return nullptr;
    }

    ObjectHandle* object = nullptr;
    error_type err = without_gil([handle = handle.get(), object_number, generation, &object] {
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
