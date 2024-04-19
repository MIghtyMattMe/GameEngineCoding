cmake_minimum_required(VERSION 3.5.0)
project(Engine1 VERSION 0.1.0 LANGUAGES C CXX)

# for filesystem functionality from C++20
set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(CTest)
enable_testing()

add_executable(Engine1
                          Brush.cpp
                          GameObject.cpp
                          EngineManager.cpp
                          main.cpp
                          UpdateFunctions/Default.cpp
                          UpdateFunctions/KeyData.h
                )

target_link_libraries(Engine1 ${OPENGL_gl_LIBRARY} ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3.lib ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3_test.lib ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3_image.lib ${CMAKE_CURRENT_SOURCE_DIR}/lib/box2d.lib)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)
