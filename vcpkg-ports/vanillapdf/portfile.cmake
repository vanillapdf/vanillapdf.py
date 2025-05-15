vcpkg_check_linkage(ONLY_DYNAMIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO vanillapdf/vanillapdf
    REF "packaging-fixes"
    SHA512 192102c0b987b7637fe4f796e572f5a348306b00768db294dd4b9ae21aa45d4ed05082a548f10f07e393188edf33581dc0ab51269ac98e082e8978ee78a4b5de
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