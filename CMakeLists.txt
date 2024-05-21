cmake_minimum_required(VERSION 3.5.0)
project(Engine1 VERSION 0.1.0 LANGUAGES C CXX)

# for filesystem functionality from C++20
set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

include(CTest)
enable_testing()

add_executable(Engine1    imgui/imgui_demo.cpp
                          imgui/imgui_draw.cpp
                          imgui/imgui_tables.cpp
                          imgui/imgui_widgets.cpp
                          imgui/imgui.cpp
                          imgui/imgui_impl_sdl3.cpp
                          imgui/imgui_impl_sdlrenderer3.cpp
                          imgui/imgui_impl_win32.cpp
                          Brush.cpp
                          GameObject.cpp
                          EngineManager.cpp
                          main.cpp
                          UpdateFunctions/KeyData.h
                          Inspector.h
                          SaveLoadBuild.h
                          UpdateFunctions/CoreFunctions.h
                          UpdateFunctions/UpdateDictionary.h
                          UpdateFunctions/ScriptConverter.h
                )

target_link_libraries(Engine1 ${OPENGL_gl_LIBRARY} ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3.lib ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3_test.lib ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3_image.lib ${CMAKE_CURRENT_SOURCE_DIR}/lib/box2d.lib)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/Debug/UpdateFunctions)
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/UpdateFunctions/ ${CMAKE_CURRENT_BINARY_DIR}/Debug/UpdateFunctions)

file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/Debug/BuildCode)
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/BuildCode/ ${CMAKE_CURRENT_BINARY_DIR}/Debug/BuildCode)
include(CPack)
