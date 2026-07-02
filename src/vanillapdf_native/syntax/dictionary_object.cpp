#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/syntax/c_object.h>
#include <vanillapdf/syntax/c_name_object.h>
#include <vanillapdf/syntax/c_dictionary_object.h>

#include "syntax/dictionary_object.h"
#include "syntax/object.h"
#include "common.h"

static const char* const ITERATOR_CAPSULE = "VanillaPDF.DictionaryObjectIterator";

PyObject* dictionary_object_create(PyObject* self, PyObject* args) {
    DictionaryObjectHandle* dictionary = nullptr;
    error_type create_err = DictionaryObject_Create(&dictionary);
    if (create_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(create_err, "DictionaryObject_Create");
    }
    auto guard = make_scope_guard([&] { DictionaryObject_Release(dictionary); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = DictionaryObject_ToObject(dictionary, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "DictionaryObject_ToObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* dictionary_object_get_size(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DictionaryObjectHandle* dictionary = object_cast<DictionaryObjectHandle>(
        capsule, DictionaryObject_FromObject, "DictionaryObject_FromObject");
    if (dictionary == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { DictionaryObject_Release(dictionary); });

    size_type size = 0;
    error_type err = DictionaryObject_GetSize(dictionary, &size);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DictionaryObject_GetSize");
    }

    return to_python(size);
}

/* Extract the DictionaryObjectHandle and the NameObjectHandle key shared by
 * find / try_find / contains / remove. Returns 0 on success (caller owns both
 * handles and must release them), -1 with an exception set on failure. */
static int extract_dictionary_and_key(PyObject* dict_capsule,
                                      PyObject* key_capsule,
                                      DictionaryObjectHandle** dictionary_out,
                                      NameObjectHandle** key_out) {
    DictionaryObjectHandle* dictionary = object_cast<DictionaryObjectHandle>(
        dict_capsule, DictionaryObject_FromObject, "DictionaryObject_FromObject");
    if (dictionary == nullptr) {
        return -1;
    }

    NameObjectHandle* key = object_cast<NameObjectHandle>(
        key_capsule, NameObject_FromObject, "NameObject_FromObject");
    if (key == nullptr) {
        DictionaryObject_Release(dictionary);
        return -1;
    }

    *dictionary_out = dictionary;
    *key_out = key;
    return 0;
}

PyObject* dictionary_object_find(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* key_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &key_capsule)) {
        return nullptr;
    }

    DictionaryObjectHandle* dictionary = nullptr;
    NameObjectHandle* key = nullptr;
    if (extract_dictionary_and_key(capsule, key_capsule, &dictionary, &key) < 0) {
        return nullptr;
    }
    auto dict_guard = make_scope_guard([&] { DictionaryObject_Release(dictionary); });
    auto key_guard = make_scope_guard([&] { NameObject_Release(key); });

    ObjectHandle* object = nullptr;
    error_type err = DictionaryObject_Find(dictionary, key, &object);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DictionaryObject_Find");
    }

    return object_capsule_from_handle(object);
}

PyObject* dictionary_object_try_find(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* key_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &key_capsule)) {
        return nullptr;
    }

    DictionaryObjectHandle* dictionary = nullptr;
    NameObjectHandle* key = nullptr;
    if (extract_dictionary_and_key(capsule, key_capsule, &dictionary, &key) < 0) {
        return nullptr;
    }
    auto dict_guard = make_scope_guard([&] { DictionaryObject_Release(dictionary); });
    auto key_guard = make_scope_guard([&] { NameObject_Release(key); });

    boolean_type found = VANILLAPDF_RV_FALSE;
    ObjectHandle* object = nullptr;
    error_type err = DictionaryObject_TryFind(dictionary, key, &found, &object);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DictionaryObject_TryFind");
    }

    if (found != VANILLAPDF_RV_TRUE) {
        Py_RETURN_NONE;
    }

    return object_capsule_from_handle(object);
}

PyObject* dictionary_object_contains(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* key_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &key_capsule)) {
        return nullptr;
    }

    DictionaryObjectHandle* dictionary = nullptr;
    NameObjectHandle* key = nullptr;
    if (extract_dictionary_and_key(capsule, key_capsule, &dictionary, &key) < 0) {
        return nullptr;
    }
    auto dict_guard = make_scope_guard([&] { DictionaryObject_Release(dictionary); });
    auto key_guard = make_scope_guard([&] { NameObject_Release(key); });

    boolean_type contains = VANILLAPDF_RV_FALSE;
    error_type err = DictionaryObject_Contains(dictionary, key, &contains);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DictionaryObject_Contains");
    }

    if (contains == VANILLAPDF_RV_TRUE) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

PyObject* dictionary_object_remove(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* key_capsule;
    if (!PyArg_ParseTuple(args, "OO", &capsule, &key_capsule)) {
        return nullptr;
    }

    DictionaryObjectHandle* dictionary = nullptr;
    NameObjectHandle* key = nullptr;
    if (extract_dictionary_and_key(capsule, key_capsule, &dictionary, &key) < 0) {
        return nullptr;
    }
    auto dict_guard = make_scope_guard([&] { DictionaryObject_Release(dictionary); });
    auto key_guard = make_scope_guard([&] { NameObject_Release(key); });

    boolean_type removed = VANILLAPDF_RV_FALSE;
    error_type err = DictionaryObject_Remove(dictionary, key, &removed);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DictionaryObject_Remove");
    }

    if (removed == VANILLAPDF_RV_TRUE) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

PyObject* dictionary_object_insert(PyObject* self, PyObject* args) {
    PyObject* capsule;
    PyObject* key_capsule;
    PyObject* value_capsule;
    int overwrite;
    if (!PyArg_ParseTuple(args, "OOOp", &capsule, &key_capsule, &value_capsule, &overwrite)) {
        return nullptr;
    }

    ObjectHandle* value = capsule_cast<ObjectHandle>(value_capsule, OBJECT_CAPSULE);
    if (value == nullptr) {
        return nullptr;
    }

    DictionaryObjectHandle* dictionary = nullptr;
    NameObjectHandle* key = nullptr;
    if (extract_dictionary_and_key(capsule, key_capsule, &dictionary, &key) < 0) {
        return nullptr;
    }
    auto dict_guard = make_scope_guard([&] { DictionaryObject_Release(dictionary); });
    auto key_guard = make_scope_guard([&] { NameObject_Release(key); });

    error_type err = DictionaryObject_Insert(
        dictionary, key, value, overwrite ? VANILLAPDF_RV_TRUE : VANILLAPDF_RV_FALSE);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DictionaryObject_Insert");
    }

    Py_RETURN_NONE;
}

PyObject* dictionary_object_clear(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DictionaryObjectHandle* dictionary = object_cast<DictionaryObjectHandle>(
        capsule, DictionaryObject_FromObject, "DictionaryObject_FromObject");
    if (dictionary == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { DictionaryObject_Release(dictionary); });

    error_type err = DictionaryObject_Clear(dictionary);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DictionaryObject_Clear");
    }

    Py_RETURN_NONE;
}

PyObject* dictionary_object_get_iterator(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DictionaryObjectHandle* dictionary = object_cast<DictionaryObjectHandle>(
        capsule, DictionaryObject_FromObject, "DictionaryObject_FromObject");
    if (dictionary == nullptr) {
        return nullptr;
    }
    auto guard = make_scope_guard([&] { DictionaryObject_Release(dictionary); });

    DictionaryObjectIteratorHandle* iterator = nullptr;
    error_type err = DictionaryObject_GetIterator(dictionary, &iterator);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DictionaryObject_GetIterator");
    }

    return capsule_new(iterator, ITERATOR_CAPSULE, RELEASE_FN(DictionaryObjectIterator_Release));
}

/* ---- Iterator ----------------------------------------------------------- */

PyObject* dictionary_iterator_get_key(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DictionaryObjectIteratorHandle* iterator =
        capsule_cast<DictionaryObjectIteratorHandle>(capsule, ITERATOR_CAPSULE);
    if (iterator == nullptr) {
        return nullptr;
    }

    NameObjectHandle* key = nullptr;
    error_type key_err = DictionaryObjectIterator_GetKey(iterator, &key);
    if (key_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(key_err, "DictionaryObjectIterator_GetKey");
    }
    auto key_guard = make_scope_guard([&] { NameObject_Release(key); });

    ObjectHandle* object = nullptr;
    error_type to_object_err = NameObject_ToObject(key, &object);
    if (to_object_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(to_object_err, "NameObject_ToObject");
    }

    return object_capsule_from_handle(object);
}

PyObject* dictionary_iterator_get_value(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DictionaryObjectIteratorHandle* iterator =
        capsule_cast<DictionaryObjectIteratorHandle>(capsule, ITERATOR_CAPSULE);
    if (iterator == nullptr) {
        return nullptr;
    }

    ObjectHandle* object = nullptr;
    error_type err = DictionaryObjectIterator_GetValue(iterator, &object);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DictionaryObjectIterator_GetValue");
    }

    return object_capsule_from_handle(object);
}

PyObject* dictionary_iterator_is_valid(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DictionaryObjectIteratorHandle* iterator =
        capsule_cast<DictionaryObjectIteratorHandle>(capsule, ITERATOR_CAPSULE);
    if (iterator == nullptr) {
        return nullptr;
    }

    boolean_type valid = VANILLAPDF_RV_FALSE;
    error_type err = DictionaryObjectIterator_IsValid(iterator, &valid);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DictionaryObjectIterator_IsValid");
    }

    if (valid == VANILLAPDF_RV_TRUE) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

PyObject* dictionary_iterator_next(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    DictionaryObjectIteratorHandle* iterator =
        capsule_cast<DictionaryObjectIteratorHandle>(capsule, ITERATOR_CAPSULE);
    if (iterator == nullptr) {
        return nullptr;
    }

    error_type err = DictionaryObjectIterator_Next(iterator);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "DictionaryObjectIterator_Next");
    }

    Py_RETURN_NONE;
}

PyObject* dictionary_iterator_release(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return nullptr;
    }

    return capsule_release(capsule, ITERATOR_CAPSULE);
}
