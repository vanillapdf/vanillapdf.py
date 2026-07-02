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
# VCPKG_GIT_TAG can be overridden (e.g. -DVCPKG_GIT_TAG=master) to build against
# vcpkg's rolling registry, as bleeding-edge.yml does
set(VCPKG_GIT_TAG "2025.12.12" CACHE STRING "vcpkg git tag, branch, or commit to fetch")

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    include(FetchContent)
    FetchContent_Declare(
        vcpkg
        GIT_REPOSITORY https://github.com/Microsoft/vcpkg.git
        GIT_TAG        ${VCPKG_GIT_TAG}
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
# VANILLAPDF_GIT_TAG can be overridden (e.g. -DVANILLAPDF_GIT_TAG=main) to build
# against the upstream library's development branch, as bleeding-edge.yml does
set(VANILLAPDF_GIT_TAG "v2.3.0-beta.1" CACHE STRING "vanillapdf git tag, branch, or commit to fetch")

include(FetchContent)
FetchContent_Declare(vanillapdf
    GIT_REPOSITORY https://github.com/vanillapdf/vanillapdf.git
    GIT_TAG        ${VANILLAPDF_GIT_TAG}
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
    src/vanillapdf_native/vanillapdfmodule.cpp
    src/vanillapdf_native/vanillapdfmodule.h
    src/vanillapdf_native/common.cpp
    src/vanillapdf_native/common.h
    src/vanillapdf_native/conversion.h

    # utils
    src/vanillapdf_native/utils/buffer.cpp
    src/vanillapdf_native/utils/buffer.h
    src/vanillapdf_native/utils/errors.cpp
    src/vanillapdf_native/utils/errors.h
    src/vanillapdf_native/utils/library_info.cpp
    src/vanillapdf_native/utils/library_info.h
    src/vanillapdf_native/utils/logging.cpp
    src/vanillapdf_native/utils/logging.h
    src/vanillapdf_native/utils/license_info.cpp
    src/vanillapdf_native/utils/license_info.h
    src/vanillapdf_native/utils/misc_utils.cpp
    src/vanillapdf_native/utils/misc_utils.h

    # syntax
    src/vanillapdf_native/syntax/file.cpp
    src/vanillapdf_native/syntax/file.h
    src/vanillapdf_native/syntax/object.cpp
    src/vanillapdf_native/syntax/object.h
    src/vanillapdf_native/syntax/null_object.cpp
    src/vanillapdf_native/syntax/null_object.h
    src/vanillapdf_native/syntax/boolean_object.cpp
    src/vanillapdf_native/syntax/boolean_object.h
    src/vanillapdf_native/syntax/integer_object.cpp
    src/vanillapdf_native/syntax/integer_object.h
    src/vanillapdf_native/syntax/real_object.cpp
    src/vanillapdf_native/syntax/real_object.h
    src/vanillapdf_native/syntax/name_object.cpp
    src/vanillapdf_native/syntax/name_object.h
    src/vanillapdf_native/syntax/string_object.cpp
    src/vanillapdf_native/syntax/string_object.h

    # semantics
    src/vanillapdf_native/semantics/document.cpp
    src/vanillapdf_native/semantics/document.h
)

# Allow includes relative to the native source root (e.g. "syntax/object.h")
target_include_directories(_vanillapdf PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vanillapdf_native
)

# C++17 for if constexpr / std::make_unsigned used in the conversion helpers
target_compile_features(_vanillapdf PRIVATE cxx_std_17)

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
