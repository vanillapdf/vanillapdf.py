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
    REF "main"
    SHA512 74d97603ed160a84430f911fc0bca679754feea63b1ff3b8e89f8f7d96a65eab11794990e692dd23889d7298366ab8200ddb7198cae498ce04d6e55b9b897239
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