#ifndef VANILLAPDF_PY_LICENSEINFOMODULE_H
#define VANILLAPDF_PY_LICENSEINFOMODULE_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* LicenseInfo_SetLicenseFile(filename: str) -> None */
PyObject* license_info_set_license_file(PyObject* self, PyObject* args);

/* LicenseInfo_SetLicenseBuffer(buffer_capsule) -> None */
PyObject* license_info_set_license_buffer(PyObject* self, PyObject* args);

/* LicenseInfo_IsValid() -> bool */
PyObject* license_info_is_valid(PyObject* self, PyObject* args);

/* LicenseInfo_IsTemporary() -> bool */
PyObject* license_info_is_temporary(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_LICENSEINFOMODULE_H */
