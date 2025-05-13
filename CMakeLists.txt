cmake_minimum_required(VERSION 3.15)

project(vanillapdf.py C CXX)

# Note: This needs to be included before defining the main project
# Check for VCPKG - C++ package management system
include(cmake/vcpkg_init.cmake)

# Find VanillaPDF via vcpkg
find_package(vanillapdf CONFIG REQUIRED)

# Find python, which is required for Python.h
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Display information about python that was found
message(STATUS "Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
message(STATUS "Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
message(STATUS "Python3_LIBRARIES: ${Python3_LIBRARIES}")

# Build the Python extension module
add_library(_vanillapdf MODULE
    src/vanillapdf_native/vanillapdfmodule.c
    src/vanillapdf_native/vanillapdfmodule.h

    src/vanillapdf_native/documentmodule.c
    src/vanillapdf_native/documentmodule.h
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

# Packaging

option(VANILLAPDF_PACKAGE_BUILD "Enable packaging install mode" OFF)

if(VANILLAPDF_PACKAGE_BUILD)

    # Path to vcpkg-installed DLLs
    set(DEPENDENCY_DLL_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")

    set(PY_PACKAGE_DIR "${PROJECT_SOURCE_DIR}/src/vanillapdf")

    install(TARGETS _vanillapdf
        LIBRARY DESTINATION ${PY_PACKAGE_DIR}  # .so (Unix)
        RUNTIME DESTINATION ${PY_PACKAGE_DIR}  # .dll (Windows)
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  # .lib/.a — not needed in wheel, but consistent
    )

    file(GLOB RUNTIME_LIBS "${DEPENDENCY_DLL_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}")
    
    install(FILES ${RUNTIME_LIBS}
        DESTINATION ${PY_PACKAGE_DIR}
    )
endif()

# Tests
enable_testing()

add_test(NAME python_wrapper_test
         COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/test_document.py)