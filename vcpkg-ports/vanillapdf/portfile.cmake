# Vanilla.PDF vcpkg portfile
# https://github.com/vanillapdf/vanillapdf
#
# Maintainer: Juraj Zikmund <jzikmund@vanillapdf.com>
# License: Apache-2.0
#
# This portfile builds Vanilla.PDF, a cross-platform SDK for creating and modifying PDF documents.
# It supports both static and shared builds via the standard CMake `BUILD_SHARED_LIBS` option.

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO vanillapdf/vanillapdf
    REF "static-library"
    SHA512 78519781d98a4da751d6c4618e0e7d61eff05a3134f2dd229fa3064952fdd320316eb89ee3c7c5d4513320fb43abc39078886d3109077bd6a0740db9a2cb2c8e
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
      -DVANILLAPDF_STANDALONE=OFF
      -DVANILLAPDF_ENABLE_TESTS=OFF
      -DVANILLAPDF_ENABLE_BENCHMARK=OFF
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(
    PACKAGE_NAME "vanillapdf"
    CONFIG_PATH "lib/cmake/vanillapdf"
)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage"
     DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

vcpkg_install_copyright(
    FILE_LIST
        "${SOURCE_PATH}/LICENSE.txt"
        "${SOURCE_PATH}/NOTICE.md"
)