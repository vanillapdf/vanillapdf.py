#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_file.h>
#include <vanillapdf/semantics/c_document.h>

/* Heap-allocated box that the capsule points to */
typedef struct {
    DocumentHandle* handle;  /* NULL => released */
} DocumentHandleBox;

static void document_capsule_destructor(PyObject* capsule) {
    DocumentHandleBox* box = (DocumentHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.Document");
    if (box != NULL) {
        if (box->handle != NULL) {
            Document_Release(box->handle);
        }
        PyMem_Free(box);
    }
}

PyObject* document_open(PyObject* self, PyObject* args) {
    const char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return NULL;
    }

    DocumentHandle* handle = NULL;
    error_type err = Document_Open(filename, &handle);

    if (err != VANILLAPDF_ERROR_SUCCESS || handle == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to open document");
        return NULL;
    }

    DocumentHandleBox* box = (DocumentHandleBox*)PyMem_Malloc(sizeof(DocumentHandleBox));
    if (box == NULL) {
        Document_Release(handle);
        return PyErr_NoMemory();
    }
    box->handle = handle;

    PyObject* capsule = PyCapsule_New((void*)box, "VanillaPDF.Document",
                                       document_capsule_destructor);
    if (capsule == NULL) {
        Document_Release(handle);
        PyMem_Free(box);
        return NULL;
    }

    return capsule;
}

PyObject* document_save(PyObject* self, PyObject* args) {
    PyObject* capsule;
    const char* filename;
    if (!PyArg_ParseTuple(args, "Os", &capsule, &filename)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, "VanillaPDF.Document")) {
        PyErr_SetString(PyExc_TypeError, "Invalid document handle");
        return NULL;
    }

    DocumentHandleBox* box = (DocumentHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.Document");
    if (box->handle == NULL) {
        PyErr_SetString(PyExc_ValueError, "Document has been released");
        return NULL;
    }

    int res = Document_Save(box->handle, filename);
    if (res != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to save document");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject* document_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }

    if (!PyCapsule_IsValid(capsule, "VanillaPDF.Document")) {
        PyErr_SetString(PyExc_TypeError, "Invalid document handle");
        return NULL;
    }

    DocumentHandleBox* box = (DocumentHandleBox*)PyCapsule_GetPointer(
        capsule, "VanillaPDF.Document");
    if (box->handle == NULL) {
        /* Already released - return success to make release idempotent */
        Py_RETURN_NONE;
    }

    Document_Release(box->handle);
    box->handle = NULL;

    Py_RETURN_NONE;
}
