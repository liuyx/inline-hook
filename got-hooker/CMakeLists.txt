cmake_minimum_required(VERSION 2.8)
project(got-hook-test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-std=c++11")

set(LIB_SRC GotHooker.cpp)

add_library(got-hook SHARED ${LIB_SRC})

set (SOURCE_FILES got-test.cpp)
add_executable(got-hook-test ${SOURCE_FILES})
target_link_libraries(got-hook-test got-hook)
