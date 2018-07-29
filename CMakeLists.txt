cmake_minimum_required(VERSION 3.7)
project(SolarSystem)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES SolarSystem.cpp SeqList.h SeqQueue.h Vector2D.h)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})

add_executable(OpenGL ${SOURCE_FILES})
target_link_libraries(OpenGL ${GLUT_LIBRARY} ${OPENGL_LIBRARY})