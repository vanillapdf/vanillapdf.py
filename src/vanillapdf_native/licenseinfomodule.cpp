#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_buffer.h>
#include <vanillapdf/utils/c_license_info.h>

#include "licenseinfomodule.h"
#include "common.h"

PyObject* license_info_set_license_file(PyObject* self, PyObject* args) {
    const char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return nullptr;
    }

    error_type err = LicenseInfo_SetLicenseFile(filename);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "LicenseInfo_SetLicenseFile");
    }

    Py_RETURN_NONE;
}

PyObject* license_info_set_license_buffer(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    BufferHandle* buffer = capsule_cast<BufferHandle>(capsule, "VanillaPDF.Buffer");
    if (buffer == nullptr) {
        return nullptr;
    }

    error_type err = LicenseInfo_SetLicenseBuffer(buffer);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "LicenseInfo_SetLicenseBuffer");
    }

    Py_RETURN_NONE;
}

PyObject* license_info_is_valid(PyObject* self, PyObject* args) {
    boolean_type is_valid = VANILLAPDF_RV_FALSE;
    error_type err = LicenseInfo_IsValid(&is_valid);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "LicenseInfo_IsValid");
    }

    if (is_valid == VANILLAPDF_RV_TRUE) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

PyObject* license_info_is_temporary(PyObject* self, PyObject* args) {
    boolean_type is_temporary = VANILLAPDF_RV_FALSE;
    error_type err = LicenseInfo_IsTemporary(&is_temporary);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "LicenseInfo_IsTemporary");
    }

    if (is_temporary == VANILLAPDF_RV_TRUE) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}
