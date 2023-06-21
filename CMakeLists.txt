cmake_minimum_required(VERSION 3.12)

project(soraxas_toolbox)

add_library(soraxas_toolbox INTERFACE)
target_include_directories(soraxas_toolbox INTERFACE include)
target_compile_features(soraxas_toolbox INTERFACE cxx_std_17)


option(BUILD_SXS_WITH_EXTERNAL_DEPS "perform a git submodule clone")
if(BUILD_SXS_WITH_EXTERNAL_DEPS)
  find_package(Git REQUIRED)
  if(NOT EXISTS include/soraxas_toolbox/external/_repo/pprint/CMakeLists.txt)
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      COMMAND_ERROR_IS_FATAL ANY)
  endif()
endif()

INSTALL (
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*")


if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  option(BUILD_SXS_WITH_TESTS "run test" ON)
else()
  option(BUILD_SXS_WITH_TESTS "run test" OFF)
endif()

if(BUILD_SXS_WITH_TESTS)
  add_subdirectory(tests)
endif()
