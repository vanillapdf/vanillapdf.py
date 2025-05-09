cmake_minimum_required(VERSION 3.14)
project(vanillapdf.py C)

# We need python headers to build python extension
#find_package(Python3 REQUIRED COMPONENTS Development)

#  # Fetch vanillapdf using FetchContent
#  include(FetchContent)
#  
#  FetchContent_Declare(
#      vanillapdf
#      GIT_REPOSITORY https://github.com/vanillapdf/vanillapdf.git
#      GIT_TAG origin/main  # or a specific tag/commit if you want
#  )
#  
#  FetchContent_MakeAvailable(vanillapdf)

# Find VanillaPDF via vcpkg
find_package(vanillapdf CONFIG REQUIRED)

# Build the Python extension module
add_library(vanillapdf.py MODULE
    src/vanillapdf_c/vanillapdfmodule.c
    src/vanillapdf_c/documentmodule.c
)

target_include_directories(vanillapdf.py PRIVATE
    ${vanillapdf_SOURCE_DIR}/include
)

target_link_libraries(vanillapdf.py PRIVATE
    vanillapdf
)

# Rename output to have .so (Linux) / .pyd (Windows) naming for Python
set_target_properties(vanillapdf.py PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)
