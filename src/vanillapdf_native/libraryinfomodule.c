#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_library_info.h>

#include "libraryinfomodule.h"

PyObject* library_info_get_version_major(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetVersionMajor(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get version major");
        return NULL;
    }
    return PyLong_FromLong((long)value);
}

PyObject* library_info_get_version_minor(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetVersionMinor(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get version minor");
        return NULL;
    }
    return PyLong_FromLong((long)value);
}

PyObject* library_info_get_version_patch(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetVersionPatch(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get version patch");
        return NULL;
    }
    return PyLong_FromLong((long)value);
}

PyObject* library_info_get_version_build(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetVersionBuild(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get version build");
        return NULL;
    }
    return PyLong_FromLong((long)value);
}

PyObject* library_info_get_author(PyObject* self, PyObject* args) {
    string_type author = NULL;
    error_type err = LibraryInfo_GetAuthor(&author);
    if (err != VANILLAPDF_ERROR_SUCCESS || author == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get author");
        return NULL;
    }
    return PyUnicode_FromString(author);
}

PyObject* library_info_get_build_day(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetBuildDay(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get build day");
        return NULL;
    }
    return PyLong_FromLong((long)value);
}

PyObject* library_info_get_build_month(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetBuildMonth(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get build month");
        return NULL;
    }
    return PyLong_FromLong((long)value);
}

PyObject* library_info_get_build_year(PyObject* self, PyObject* args) {
    integer_type value = 0;
    error_type err = LibraryInfo_GetBuildYear(&value);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get build year");
        return NULL;
    }
    return PyLong_FromLong((long)value);
}
