#ifndef VANILLAPDF_PY_SYNTAX_DICTIONARY_OBJECT_H
#define VANILLAPDF_PY_SYNTAX_DICTIONARY_OBJECT_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* Dictionary object. Keys are passed/returned as Name "VanillaPDF.Object"
 * capsules; values as "VanillaPDF.Object" capsules. */
PyObject* dictionary_object_create(PyObject* self, PyObject* args);
PyObject* dictionary_object_get_size(PyObject* self, PyObject* args);
PyObject* dictionary_object_find(PyObject* self, PyObject* args);
PyObject* dictionary_object_try_find(PyObject* self, PyObject* args);
PyObject* dictionary_object_contains(PyObject* self, PyObject* args);
PyObject* dictionary_object_get_iterator(PyObject* self, PyObject* args);
PyObject* dictionary_object_remove(PyObject* self, PyObject* args);
PyObject* dictionary_object_clear(PyObject* self, PyObject* args);
PyObject* dictionary_object_insert(PyObject* self, PyObject* args);

/* Dictionary iterator ("VanillaPDF.DictionaryObjectIterator" capsule). */
PyObject* dictionary_iterator_get_key(PyObject* self, PyObject* args);
PyObject* dictionary_iterator_get_value(PyObject* self, PyObject* args);
PyObject* dictionary_iterator_is_valid(PyObject* self, PyObject* args);
PyObject* dictionary_iterator_next(PyObject* self, PyObject* args);
PyObject* dictionary_iterator_release(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SYNTAX_DICTIONARY_OBJECT_H */
