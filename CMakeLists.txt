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

# This implies we're building a Python extension, not a shared lib
if(BUILD_SHARED_LIBS)
    message(STATUS "VanillaPDF is linked dynamically; copying DLL dependencies")

    # Path to vcpkg-installed DLLs
    set(DEPENDENCY_DLL_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")

    # List DLLs that must accompany _vanillapdf.pyd
    install(FILES
        "${DEPENDENCY_DLL_DIR}/libvanillapdf.dll"
        "${DEPENDENCY_DLL_DIR}/spdlog.dll"
        "${DEPENDENCY_DLL_DIR}/openjp2.dll"
        "${DEPENDENCY_DLL_DIR}/zlib1.dll"
        "${DEPENDENCY_DLL_DIR}/jpeg62.dll"
        "${DEPENDENCY_DLL_DIR}/libcrypto-3-x64.dll"
        "${DEPENDENCY_DLL_DIR}/legacy.dll"
        "${DEPENDENCY_DLL_DIR}/fmt.dll"
        # Add more DLLs here if needed
        DESTINATION ${SKBUILD_PLATLIB_DIR}/vanillapdf
    )
endif()

# Tests
enable_testing()

add_test(NAME python_wrapper_test
         COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/test_document.py)