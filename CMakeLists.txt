cmake_minimum_required(VERSION 3.20.0)
project(openglGlutSandbox CXX)

add_subdirectory(Projects)
add_executable(openglGlutSandbox main.cpp)

target_link_libraries(openglGlutSandbox openglGlutSandboxLib)
