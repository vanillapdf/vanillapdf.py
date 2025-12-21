cmake_minimum_required(VERSION 3.20)

# Static/dynamic linking configuration
option(BUILD_SHARED_LIBS "Build shared libraries instead of static ones" OFF)

if(BUILD_SHARED_LIBS)
    set(VCPKG_LIBRARY_LINKAGE "dynamic")
else()
    set(VCPKG_LIBRARY_LINKAGE "static")

    # Windows with static linking requires a different vcpkg triplet
    if(WIN32 AND NOT DEFINED VCPKG_TARGET_TRIPLET)
        set(VCPKG_TARGET_TRIPLET "x64-windows-static-md")
    endif()
endif()

# Set up vcpkg toolchain before project() call
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    include(FetchContent)
    FetchContent_Declare(
        vcpkg
        GIT_REPOSITORY https://github.com/Microsoft/vcpkg.git
        GIT_TAG        2025.12.12
        SOURCE_DIR     ${CMAKE_BINARY_DIR}/vcpkg
    )
    FetchContent_MakeAvailable(vcpkg)

    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_BINARY_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    message(STATUS "Set up vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()

# Add option to build a pypi package
option(VANILLAPDF_PY_PACKAGE_BUILD "Enable packaging install mode" OFF)

project(vanillapdf.py C CXX)

# Fetch VanillaPDF library
include(FetchContent)
FetchContent_Declare(vanillapdf
    GIT_REPOSITORY https://github.com/vanillapdf/vanillapdf.git
    GIT_TAG        v2.2.0
    GIT_SUBMODULES ""  # Don't fetch submodules (we have our own vcpkg)
)

# Configure vanillapdf to use parent's vcpkg
set(VANILLAPDF_INTERNAL_VCPKG OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Build with -fPIC" FORCE)

FetchContent_MakeAvailable(vanillapdf)

# Find python, which is required for Python.h
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)

# Build the Python extension module
add_library(_vanillapdf MODULE
    src/vanillapdf_native/vanillapdfmodule.c
    src/vanillapdf_native/vanillapdfmodule.h

    src/vanillapdf_native/documentmodule.c
    src/vanillapdf_native/documentmodule.h
    src/vanillapdf_native/buffermodule.c
    src/vanillapdf_native/buffermodule.h
    src/vanillapdf_native/filemodule.c
    src/vanillapdf_native/filemodule.h
    src/vanillapdf_native/libraryinfomodule.c
    src/vanillapdf_native/libraryinfomodule.h
    src/vanillapdf_native/loggingmodule.c
    src/vanillapdf_native/loggingmodule.h
)

target_link_libraries(_vanillapdf PRIVATE
    vanillapdf::vanillapdf
    Python3::Module
)

# Specify target suffix based on the current platform
if(WIN32)
    set(VANILLAPDF_PY_TARGET_SUFFIX ".pyd")
elseif(APPLE)
    set(VANILLAPDF_PY_TARGET_SUFFIX ".so")
elseif(UNIX)
    set(VANILLAPDF_PY_TARGET_SUFFIX ".so")
else()
    message(FATAL_ERROR "Unknown host platform")
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
    include(GNUInstallDirs)

    install(TARGETS _vanillapdf
        LIBRARY DESTINATION vanillapdf  # .so (Unix)
        RUNTIME DESTINATION vanillapdf  # .dll (Windows)
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    # Set RPATH for Unix platforms
    if(UNIX AND NOT APPLE)
        set_target_properties(_vanillapdf PROPERTIES
            INSTALL_RPATH "$ORIGIN"
        )
    elseif(APPLE)
        set_target_properties(_vanillapdf PROPERTIES
            INSTALL_RPATH "@loader_path"
            BUILD_RPATH "@loader_path"
        )
    endif()
endif()

# Tests
enable_testing()

add_test(NAME python_wrapper_test
         COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/test_document.py)
