cmake_minimum_required(VERSION 3.20.0)
project(openglGlutSandboxLib CXX)

macro (collect var pattern)
    file(GLOB files ${pattern})
    foreach(file ${files})
        set(${var} ${${var}} ${file})
    endforeach()
endmacro()

collect(HEADERS "*.h")

add_library(openglGlutSandboxLib OBJECT
    ${HEADERS}
)

find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(openglGlutSandboxLib PUBLIC ${GLEW_INCLUDE_DIRS}
                                          PUBLIC ${GLUT_INCLUDE_DIRS}
                                          PUBLIC ${OPENGL_INCLUDE_DIRS}
                                          PUBLIC .
                                          )
                                          
target_link_libraries(openglGlutSandboxLib ${GLEW_LIBRARIES} ${GLUT_LIBRARY} ${OPENGL_LIBRARIES})
set_target_properties(openglGlutSandboxLib PROPERTIES LINKER_LANGUAGE CXX)
