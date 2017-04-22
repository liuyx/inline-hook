include_directories(${PROJECT_SOURCE_DIR}/libs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-std=c++11")

set(SOURCE_FILES main.cpp)

add_executable(hooker-test ${SOURCE_FILES})
target_link_libraries(hooker-test inline-hook)
