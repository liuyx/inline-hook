cmake_minimum_required(VERSION 3.6)
project(c__11)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Hooker.cpp Hooker.h HookerFactory.cpp HookerFactory.h HookerX64.cpp HookerX64.h HookerX86.cpp HookerX86.h HookerArm.cpp HookerArm.h HookerThumb.cpp HookerThumb.h)
add_executable(c__11 ${SOURCE_FILES})
