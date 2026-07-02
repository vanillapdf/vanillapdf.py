#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_library_info.h>

#include "libraryinfomodule.h"
#include "common.h"

PyObject* library_info_get_version_major(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetVersionMajor(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "LibraryInfo_GetVersionMajor");
    }
    return PyLong_FromLong((long)value);
}

PyObject* library_info_get_version_minor(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetVersionMinor(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "LibraryInfo_GetVersionMinor");
    }
    return PyLong_FromLong((long)value);
}

PyObject* library_info_get_version_patch(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetVersionPatch(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "LibraryInfo_GetVersionPatch");
    }
    return PyLong_FromLong((long)value);
}

PyObject* library_info_get_version_build(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetVersionBuild(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "LibraryInfo_GetVersionBuild");
    }
    return PyLong_FromLong((long)value);
}

PyObject* library_info_get_author(PyObject* self, PyObject* args) {
    string_type author = nullptr;
    error_type err = LibraryInfo_GetAuthor(&author);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "LibraryInfo_GetAuthor");
    }
    return PyUnicode_FromString(author);
}

PyObject* library_info_get_build_day(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetBuildDay(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "LibraryInfo_GetBuildDay");
    }
    return PyLong_FromLong((long)value);
}

PyObject* library_info_get_build_month(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetBuildMonth(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "LibraryInfo_GetBuildMonth");
    }
    return PyLong_FromLong((long)value);
}

PyObject* library_info_get_build_year(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetBuildYear(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "LibraryInfo_GetBuildYear");
    }
    return PyLong_FromLong((long)value);
}
