vcpkg_check_linkage(ONLY_DYNAMIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO vanillapdf/vanillapdf
    REF "main"
    SHA512 fd49a8020381f8a9a1adb7fb90a03309637680669c81ecc09e5a38b264521029c19da8c4d1e298bb8efd485eb9c28b15fd398dc135efe7920d04c126cd0f0f38
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