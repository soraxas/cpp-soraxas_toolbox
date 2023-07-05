include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
  doctest
  PREFIX ${CMAKE_BINARY_DIR}/doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  TIMEOUT 10
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON)

# Expose required variable (DOCTEST_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(doctest source_dir)
set(DOCTEST_INCLUDE_DIR
    ${source_dir}/doctest
    CACHE INTERNAL "Path to include folder for doctest")

# Make test executable add_executable(tests main.cpp)
add_executable(tests tests_runner.cpp)
target_compile_features(tests PRIVATE cxx_std_17)
target_link_libraries(tests PRIVATE soraxas_toolbox)
target_include_directories(tests PUBLIC ${DOCTEST_INCLUDE_DIR})
add_dependencies(tests doctest)
enable_testing()

find_package(Eigen3)
if(Eigen3_FOUND)
  add_compile_definitions(HAS_EIGEN_=TRUE)
  target_link_libraries(tests PRIVATE Eigen3::Eigen)
endif()
