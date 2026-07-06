#ifndef VANILLAPDF_PY_UTILS_MISC_UTILS_H
#define VANILLAPDF_PY_UTILS_MISC_UTILS_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* MiscUtils_InitializeOpenSSL() -> None */
PyObject* misc_utils_initialize_openssl(PyObject* self, PyObject* args);

/* MiscUtils_CleanupOpenSSL() -> None */
PyObject* misc_utils_cleanup_openssl(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_UTILS_MISC_UTILS_H */
