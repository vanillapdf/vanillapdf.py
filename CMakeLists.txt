cmake_minimum_required(VERSION 3.15)

project(vanillapdf.py C CXX)

# Note: This needs to be included before defining the main project
# Check for VCPKG - C++ package management system
include(cmake/vcpkg_init.cmake)

# Find VanillaPDF via vcpkg
find_package(vanillapdf CONFIG REQUIRED)

# Find python, which is required for Python.h
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Build the Python extension module
add_library(_vanillapdf MODULE
    src/vanillapdf_c/vanillapdfmodule.c
    src/vanillapdf_c/documentmodule.c
)

target_link_libraries(_vanillapdf PRIVATE
    vanillapdf::vanillapdf
    Python3::Python
)

# Correct cross-platform suffix
if(WIN32)
    set_target_properties(_vanillapdf PROPERTIES SUFFIX ".pyd")
else()
    set_target_properties(_vanillapdf PROPERTIES SUFFIX ".so")
endif()

message(STATUS "Python extension will be built as: ${CMAKE_CURRENT_BINARY_DIR}/_vanillapdf${CMAKE_SHARED_LIBRARY_SUFFIX}")

# Install target to copy built extension to the Python module dir
install(TARGETS _vanillapdf
        LIBRARY DESTINATION ${SKBUILD_PLATLIB_DIR}/vanillapdf
        RUNTIME DESTINATION ${SKBUILD_PLATLIB_DIR}/vanillapdf
        ARCHIVE DESTINATION ${SKBUILD_PLATLIB_DIR}/vanillapdf)

# Tests
enable_testing()

add_test(NAME python_wrapper_test
         COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/test_document.py)