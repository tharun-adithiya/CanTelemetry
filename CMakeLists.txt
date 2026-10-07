cmake_minimum_required(VERSION 3.20)
project(CanTelemetry CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#Enforces clean, strict systems code
add_compile_options(-Wall -Wextra -Wpedantic -O2 -pthread)
#Includes directory for header files
include_directories(include)
#Executable Target
add_executable(can_receiver 
    src/main.cpp
    src/CanSocket.cpp
    src/FrameDecoder.cpp
)
target_link_libraries(can_receiver PRIVATE pthread)