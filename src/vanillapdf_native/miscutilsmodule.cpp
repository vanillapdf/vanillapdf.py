#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_misc_utils.h>

#include "miscutilsmodule.h"
#include "common.h"

PyObject* misc_utils_initialize_openssl(PyObject* self, PyObject* args) {
    error_type err = MiscUtils_InitializeOpenSSL();
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "MiscUtils_InitializeOpenSSL");
    }

    Py_RETURN_NONE;
}

PyObject* misc_utils_cleanup_openssl(PyObject* self, PyObject* args) {
    error_type err = MiscUtils_CleanupOpenSSL();
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "MiscUtils_CleanupOpenSSL");
    }

    Py_RETURN_NONE;
}
