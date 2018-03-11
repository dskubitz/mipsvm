project(vm)

set(CMAKE_CXX_STANDARD 14)

add_executable(${PROJECT_NAME} VM.h VM.cpp)
include_directories(${common_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} common)
