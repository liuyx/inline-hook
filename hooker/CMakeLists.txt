cmake_minimum_required(VERSION 2.8)
project(hooker-test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-std=c++11")

set(LIB_SRC Hooker.cpp Hooker.h HookerFactory.cpp HookerFactory.h HookerX64.cpp HookerX64.h HookerX86.cpp HookerX86.h HookerArm.cpp HookerArm.h HookerThumb.cpp HookerThumb.h)

add_library(inline-hook SHARED ${LIB_SRC})
