#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/syntax/c_dictionary_object.h>
#include <vanillapdf/semantics/c_document.h>
#include <vanillapdf/semantics/c_page_object.h>
#include <vanillapdf/semantics/c_rectangle.h>

#include "semantics/page_object.h"
#include "semantics/rectangle.h"
#include "syntax/object.h"
#include "common.h"

static const char* const PAGE_OBJECT_CAPSULE = "VanillaPDF.PageObject";

PyObject* page_object_capsule_from_handle(PageObjectHandle* handle) {
    return capsule_new(handle, PAGE_OBJECT_CAPSULE, RELEASE_FN(PageObject_Release));
}

PyObject* page_object_create_from_document(PyObject* self, PyObject* args) {
    PyObject* document_capsule;
    if (!PyArg_ParseTuple(args, "O", &document_capsule)) {
        return nullptr;
    }

    auto document = capsule_cast<DocumentHandle>(document_capsule, "VanillaPDF.Document");
    if (!document) {
        return nullptr;
    }

    PageObjectHandle* page = nullptr;
    error_type err = without_gil([document = document.get(), &page] {
        return PageObject_CreateFromDocument(document, &page);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "PageObject_CreateFromDocument");
    }

    return page_object_capsule_from_handle(page);
}

PyObject* page_object_get_media_box(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    auto page = capsule_cast<PageObjectHandle>(capsule, PAGE_OBJECT_CAPSULE);
    if (!page) {
        return nullptr;
    }

    RectangleHandle* media_box = nullptr;
    error_type err = without_gil([page = page.get(), &media_box] {
        return PageObject_GetMediaBox(page, &media_box);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "PageObject_GetMediaBox");
    }

    return rectangle_capsule_from_handle(media_box);
}

PyObject* page_object_set_media_box(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* rectangle_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &rectangle_capsule)) {
        return nullptr;
    }

    auto media_box = capsule_cast<RectangleHandle>(rectangle_capsule, "VanillaPDF.Rectangle");
    if (!media_box) {
        return nullptr;
    }

    auto page = capsule_cast<PageObjectHandle>(capsule, PAGE_OBJECT_CAPSULE);
    if (!page) {
        return nullptr;
    }

    error_type err = without_gil([page = page.get(), media_box = media_box.get()] {
        return PageObject_SetMediaBox(page, media_box);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "PageObject_SetMediaBox");
    }

    Py_RETURN_NONE;
}

PyObject* page_object_get_base_object(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    auto page = capsule_cast<PageObjectHandle>(capsule, PAGE_OBJECT_CAPSULE);
    if (!page) {
        return nullptr;
    }

    DictionaryObjectHandle* base = nullptr;
    error_type base_err = without_gil([page = page.get(), &base] {
        return PageObject_GetBaseObject(page, &base);
    });
    if (base_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(base_err, "PageObject_GetBaseObject");
    }
    SCOPE_GUARD([base] { release_without_gil(DictionaryObject_Release, base); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = without_gil([base, &object] {
        return DictionaryObject_ToObject(base, &object);
    });
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "DictionaryObject_ToObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* page_object_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, PAGE_OBJECT_CAPSULE);
}
