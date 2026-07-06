#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/semantics/c_interactive_forms.h>

#include "semantics/interactive_form.h"
#include "semantics/fields.h"
#include "common.h"

static const char* const INTERACTIVE_FORM_CAPSULE = "VanillaPDF.InteractiveForm";

PyObject* interactive_form_capsule_from_handle(InteractiveFormHandle* handle) {
    return capsule_new(handle, INTERACTIVE_FORM_CAPSULE, RELEASE_FN(InteractiveForm_Release));
}

PyObject* interactive_form_get_fields(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    InteractiveFormHandle* form =
        capsule_cast<InteractiveFormHandle>(capsule, INTERACTIVE_FORM_CAPSULE);
    if (form == nullptr) {
        return nullptr;
    }

    FieldCollectionHandle* fields = nullptr;
    error_type err = InteractiveForm_GetFields(form, &fields);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "InteractiveForm_GetFields");
    }

    return field_collection_capsule_from_handle(fields);
}

PyObject* interactive_form_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, INTERACTIVE_FORM_CAPSULE);
}
