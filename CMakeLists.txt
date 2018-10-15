cmake_minimum_required(VERSION 3.0.0)
project(maintest VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

include_directories(./include)
# aux_source_directory(./test DIR_ROOT_SRCS)

add_subdirectory(src)

add_executable(maintest ./test/test.cpp)
# add_executable(maintest ./test/formatcsv.cpp)

target_link_libraries(maintest libtradingcore)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
