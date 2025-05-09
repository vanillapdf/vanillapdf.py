# VCPKG - C++ package management system

if(NOT VANILLAPDF_PY_STANDALONE)
  message(STATUS "VanillaPDF is being built as a vcpkg port or embedded, skipping automatic package management")
  return()
endif()

if(NOT DEFINED VCPKG_ROOT)
  set(VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg")
endif()

if(WIN32)
  set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg.exe)
  set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.bat)
else()
  set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg)
  set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.sh)
endif()

if(NOT EXISTS ${VCPKG_EXEC})
  message(STATUS "Bootstrapping vcpkg in ${VCPKG_ROOT}")
  execute_process(COMMAND ${VCPKG_BOOTSTRAP})
endif()

if(NOT EXISTS ${VCPKG_EXEC})
  message(FATAL_ERROR "***** FATAL ERROR: Could not bootstrap vcpkg *****")
endif()
