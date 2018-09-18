cmake_minimum_required(VERSION 3.0.0)
project(maintest VERSION 0.1.0)

include(CTest)
enable_testing()

aux_source_directory(./test DIR_ROOT_SRCS)

add_executable(maintest ./test/test.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
