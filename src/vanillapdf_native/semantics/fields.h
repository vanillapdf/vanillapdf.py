#ifndef VANILLAPDF_PY_SEMANTICS_FIELDS_H
#define VANILLAPDF_PY_SEMANTICS_FIELDS_H

#include <Python.h>
#include <vanillapdf/semantics/c_fields.h>

/* Wrap a FieldCollectionHandle in a capsule (takes ownership). Public so the
 * interactive-form module can return the form's fields. */
PyObject* field_collection_capsule_from_handle(FieldCollectionHandle* handle);

/* FieldCollection ("VanillaPDF.FieldCollection") */
PyObject* field_collection_get_size(PyObject* self, PyObject* args);
PyObject* field_collection_at(PyObject* self, PyObject* args);
PyObject* field_collection_release(PyObject* self, PyObject* args);

/* Field ("VanillaPDF.Field") */
PyObject* field_get_type(PyObject* self, PyObject* args);
PyObject* field_release(PyObject* self, PyObject* args);

/* SignatureField ("VanillaPDF.SignatureField") */
PyObject* signature_field_from_field(PyObject* self, PyObject* args);
PyObject* signature_field_get_value(PyObject* self, PyObject* args);
PyObject* signature_field_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SEMANTICS_FIELDS_H */
