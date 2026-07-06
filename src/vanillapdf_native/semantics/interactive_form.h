#ifndef VANILLAPDF_PY_SEMANTICS_INTERACTIVE_FORM_H
#define VANILLAPDF_PY_SEMANTICS_INTERACTIVE_FORM_H

#include <Python.h>
#include <vanillapdf/semantics/c_interactive_forms.h>

/* Wrap an InteractiveFormHandle in a capsule (takes ownership). Public so the
 * catalog module can return the document's AcroForm. */
PyObject* interactive_form_capsule_from_handle(InteractiveFormHandle* handle);

PyObject* interactive_form_get_fields(PyObject* self, PyObject* args);
PyObject* interactive_form_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SEMANTICS_INTERACTIVE_FORM_H */
