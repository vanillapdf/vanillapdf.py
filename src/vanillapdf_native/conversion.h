#ifndef VANILLAPDF_PY_CONVERSION_H
#define VANILLAPDF_PY_CONVERSION_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <cstdint>
#include <limits>
#include <type_traits>

/* Value conversion helpers, kept out of common.h so the range-checking logic
 * does not clutter it. Modeled on vanillapdf's utils/conversion_utils.h.
 *
 * Call sites only use the simple interface below:
 *   error_type code;   if (safe_convert(raw, &code) < 0) return nullptr;
 *   return to_python(value);
 */

/* True if integral `value` is representable in integral type To. Split by
 * signedness so the compiler never compares a signed against an unsigned (the
 * same reason vanillapdf's ValueConvertUtils splits into specializations). */
template <typename To, typename From>
bool value_in_range(From value) {
    static_assert(std::is_integral<To>::value && std::is_integral<From>::value,
                  "value_in_range requires integral types");

    if constexpr (std::is_signed<From>::value == std::is_signed<To>::value) {
        return value >= std::numeric_limits<To>::min()
            && value <= std::numeric_limits<To>::max();
    } else if constexpr (std::is_signed<From>::value) {
        // signed source, unsigned target
        return value >= 0
            && static_cast<typename std::make_unsigned<From>::type>(value)
                   <= std::numeric_limits<To>::max();
    } else {
        // unsigned source, signed target
        return value <= static_cast<typename std::make_unsigned<To>::type>(
                            std::numeric_limits<To>::max());
    }
}

/* Range-checked integral conversion; use instead of a narrowing cast. Stores
 * into *out and returns 0, or sets OverflowError and returns -1 if `value`
 * does not fit in To. */
template <typename To, typename From>
int safe_convert(From value, To* out) {
    if (!value_in_range<To>(value)) {
        PyErr_SetString(PyExc_OverflowError,
                        "integer value out of range for target type");
        return -1;
    }
    *out = static_cast<To>(value);
    return 0;
}

/* Build a new Python object from a native scalar without a cast at the call
 * site. Overloaded on the library's fixed-width value types; unscoped enums
 * match the int32_t overload via promotion. */
inline PyObject* to_python(int32_t value)  { return PyLong_FromLong(value); }
inline PyObject* to_python(int64_t value)  { return PyLong_FromLongLong(value); }
inline PyObject* to_python(uint32_t value) { return PyLong_FromUnsignedLong(value); }
inline PyObject* to_python(uint64_t value) { return PyLong_FromUnsignedLongLong(value); }
inline PyObject* to_python(double value)   { return PyFloat_FromDouble(value); }

#endif /* VANILLAPDF_PY_CONVERSION_H */
