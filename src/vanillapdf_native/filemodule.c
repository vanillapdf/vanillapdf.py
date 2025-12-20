#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_file.h>
#include <vanillapdf/utils/c_pdf_version.h>

#include "filemodule.h"
#include "buffermodule.h"

/* Heap-allocated box that the capsule points to */
typedef struct {
    FileHandle* handle;  /* NULL => released */
} FileHandleBox;

static void file_capsule_destructor(PyObject* capsule) {
    FileHandleBox* box = (FileHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.File");
    if (box != NULL) {
        if (box->handle != NULL) {
            File_Release(box->handle);
        }
        PyMem_Free(box);
    }
}

PyObject* file_open(PyObject* self, PyObject* args) {
    const char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return NULL;
    }

    FileHandle* handle = NULL;
    error_type err = File_Open(filename, &handle);

    if (err != VANILLAPDF_ERROR_SUCCESS || handle == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to open file");
        return NULL;
    }

    FileHandleBox* box = (FileHandleBox*)PyMem_Malloc(sizeof(FileHandleBox));
    if (box == NULL) {
        File_Release(handle);
        return PyErr_NoMemory();
    }
    box->handle = handle;

    PyObject* capsule = PyCapsule_New((void*)box, "VanillaPDF.File",
                                       file_capsule_destructor);
    if (capsule == NULL) {
        File_Release(handle);
        PyMem_Free(box);
        return NULL;
    }

    return capsule;
}

PyObject* file_initialize(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, "VanillaPDF.File")) {
        PyErr_SetString(PyExc_TypeError, "Invalid file handle");
        return NULL;
    }

    FileHandleBox* box = (FileHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.File");
    if (box->handle == NULL) {
        PyErr_SetString(PyExc_ValueError, "File has been released");
        return NULL;
    }

    error_type err = File_Initialize(box->handle);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to initialize file");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject* file_get_version(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, "VanillaPDF.File")) {
        PyErr_SetString(PyExc_TypeError, "Invalid file handle");
        return NULL;
    }

    FileHandleBox* box = (FileHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.File");
    if (box->handle == NULL) {
        PyErr_SetString(PyExc_ValueError, "File has been released");
        return NULL;
    }

    PDFVersion version;
    error_type err = File_GetVersion(box->handle, &version);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get file version");
        return NULL;
    }

    /* Convert version enum to string */
    const char* version_str;
    switch (version) {
        case PDFVersion_10: version_str = "1.0"; break;
        case PDFVersion_11: version_str = "1.1"; break;
        case PDFVersion_12: version_str = "1.2"; break;
        case PDFVersion_13: version_str = "1.3"; break;
        case PDFVersion_14: version_str = "1.4"; break;
        case PDFVersion_15: version_str = "1.5"; break;
        case PDFVersion_16: version_str = "1.6"; break;
        case PDFVersion_17: version_str = "1.7"; break;
        case PDFVersion_20: version_str = "2.0"; break;
        default: version_str = "unknown"; break;
    }

    return PyUnicode_FromString(version_str);
}

PyObject* file_get_filename(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, "VanillaPDF.File")) {
        PyErr_SetString(PyExc_TypeError, "Invalid file handle");
        return NULL;
    }

    FileHandleBox* box = (FileHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.File");
    if (box->handle == NULL) {
        PyErr_SetString(PyExc_ValueError, "File has been released");
        return NULL;
    }

    BufferHandle* buffer = NULL;
    error_type err = File_GetFilename(box->handle, &buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS || buffer == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get filename");
        return NULL;
    }

    return buffer_capsule_from_handle(buffer);
}

PyObject* file_is_encrypted(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, "VanillaPDF.File")) {
        PyErr_SetString(PyExc_TypeError, "Invalid file handle");
        return NULL;
    }

    FileHandleBox* box = (FileHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.File");
    if (box->handle == NULL) {
        PyErr_SetString(PyExc_ValueError, "File has been released");
        return NULL;
    }

    boolean_type is_encrypted = VANILLAPDF_RV_FALSE;
    error_type err = File_IsEncrypted(box->handle, &is_encrypted);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to check encryption status");
        return NULL;
    }

    if (is_encrypted == VANILLAPDF_RV_TRUE) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

PyObject* file_set_encryption_password(PyObject* self, PyObject* args) {
    PyObject* capsule;
    const char* password;
    if (!PyArg_ParseTuple(args, "Os", &capsule, &password)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, "VanillaPDF.File")) {
        PyErr_SetString(PyExc_TypeError, "Invalid file handle");
        return NULL;
    }

    FileHandleBox* box = (FileHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.File");
    if (box->handle == NULL) {
        PyErr_SetString(PyExc_ValueError, "File has been released");
        return NULL;
    }

    error_type err = File_SetEncryptionPassword(box->handle, password);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to set encryption password");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject* file_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, "VanillaPDF.File")) {
        PyErr_SetString(PyExc_TypeError, "Invalid file handle");
        return NULL;
    }

    FileHandleBox* box = (FileHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.File");
    if (box->handle == NULL) {
        /* Already released - return success to make release idempotent */
        Py_RETURN_NONE;
    }

    File_Release(box->handle);
    box->handle = NULL;

    Py_RETURN_NONE;
}
