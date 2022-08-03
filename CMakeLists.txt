cmake_minimum_required(VERSION 3.22)

project(soraxas_toolbox)

add_library(soraxas_toolbox INTERFACE)
target_include_directories(soraxas_toolbox INTERFACE include)
target_compile_features(soraxas_toolbox INTERFACE cxx_std_17)

INSTALL(TARGETS soraxas_toolbox
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
