cmake_minimum_required(VERSION 3.15)

# Add option to build a pypi package
option(VANILLAPDF_PY_PACKAGE_BUILD "Enable packaging install mode" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static ones" OFF)

if (BUILD_SHARED_LIBS)
    set(VCPKG_LIBRARY_LINKAGE "dynamic")
else()
    set(VCPKG_LIBRARY_LINKAGE "static")
    set(VCPKG_TARGET_TRIPLET "x64-windows-static")
endif()

# Note: This needs to be included before defining the main project
# Check for VCPKG - C++ package management system
include(cmake/vcpkg_init.cmake)

project(vanillapdf.py C CXX)

# Find VanillaPDF via vcpkg
find_package(vanillapdf CONFIG REQUIRED)

# Find python, which is required for Python.h
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)

# Build the Python extension module
add_library(_vanillapdf MODULE
    src/vanillapdf_native/vanillapdfmodule.c
    src/vanillapdf_native/vanillapdfmodule.h

    src/vanillapdf_native/documentmodule.c
    src/vanillapdf_native/documentmodule.h
)

target_link_libraries(_vanillapdf PRIVATE
    vanillapdf::vanillapdf
    Python3::Module
)

# Correct cross-platform suffix
set(VANILLAPDF_PY_TARGET_SUFFIX ".so")

# Specific case on windows it is different
if(WIN32)
    set(VANILLAPDF_PY_TARGET_SUFFIX ".pyd")
endif()

# Disable the lib prefix, which is not common for python modules
set_target_properties(_vanillapdf PROPERTIES
    OUTPUT_NAME "_vanillapdf"
    PREFIX ""
    SUFFIX ${VANILLAPDF_PY_TARGET_SUFFIX}
)

message(STATUS "Python extension will be built as: ${CMAKE_CURRENT_BINARY_DIR}/_vanillapdf${CMAKE_SHARED_LIBRARY_SUFFIX}")

# Packaging

if(VANILLAPDF_PY_PACKAGE_BUILD)

    # Required to get CMAKE_INSTALL_LIBDIR
    include(GNUInstallDirs)

    install(TARGETS _vanillapdf
        LIBRARY DESTINATION vanillapdf  # .so (Unix)
        RUNTIME DESTINATION vanillapdf  # .dll (Windows)
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  # .lib/.a — not needed in wheel, but consistent
    )
    
    # Copy dependent shared libraries (Linux)
    if(UNIX AND NOT APPLE)

        # Set RPATH to look in the same folder
        set_target_properties(_vanillapdf PROPERTIES
            INSTALL_RPATH "$ORIGIN"
        )

        file(GLOB VCPKG_SOS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/*.so*")
        install(FILES ${VCPKG_SOS} DESTINATION vanillapdf)
    endif()
    
    # Copy dylibs (macOS)
    if(APPLE)
        set_target_properties(_vanillapdf PROPERTIES
            INSTALL_RPATH "@loader_path"
            BUILD_RPATH "@loader_path"
        )

        file(GLOB VCPKG_DYLIBS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/*.dylib")
        install(FILES ${VCPKG_DYLIBS} DESTINATION vanillapdf)
    endif()
    
    # Copy dlls (Windows)
    if(WIN32)
        file(GLOB VCPKG_DLLS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll")
        install(FILES ${VCPKG_DLLS} DESTINATION vanillapdf)
    endif()
    


endif()

# Tests
enable_testing()

add_test(NAME python_wrapper_test
         COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/test_document.py)