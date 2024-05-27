cmake_minimum_required(VERSION 3.5.0)
project(Engine1 VERSION 0.1.0 LANGUAGES C CXX)

# for filesystem functionality from C++20
set(CMAKE_CXX_STANDARD 20)

set(ENGINE_CODE ON)
if(ENGINE_CODE)
    add_compile_definitions(ENGINE_CODE)
endif()

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include 
if(ENGINE_CODE)
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
endif()
)


include(CTest)
enable_testing()
if(ENGINE_CODE)
    add_executable(Engine1    
            imgui/imgui_demo.cpp
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
else()
    add_executable(Engine1                      
            Brush.cpp
            GameObject.cpp
            EngineManager.cpp
            main.cpp
            UpdateFunctions/KeyData.h
            SaveLoadBuild.h
            UpdateFunctions/CoreFunctions.h
            UpdateFunctions/UpdateDictionary.h
            UpdateFunctions/ScriptConverter.h
            )
endif()

target_link_libraries(Engine1 ${OPENGL_gl_LIBRARY} ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3.lib ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3_test.lib ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3_image.lib ${CMAKE_CURRENT_SOURCE_DIR}/lib/box2d.lib)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

if(ENGINE_CODE)
    #override the current BuildCode directory with my (updated) code
    file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/BuildCode)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/include/ ${CMAKE_SOURCE_DIR}/BuildCode/include)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/lib/ ${CMAKE_SOURCE_DIR}/BuildCode/lib)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/UpdateFunctions/ ${CMAKE_SOURCE_DIR}/BuildCode/UpdateFunctions)
    file(COPY ${CMAKE_SOURCE_DIR}/*.h DESTINATION ${CMAKE_SOURCE_DIR}/BuildCode)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy
                        ${CMAKE_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/BuildCode)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy
                        ${CMAKE_SOURCE_DIR}/CMakeLists.txt ${CMAKE_SOURCE_DIR}/BuildCode)

    #move the updatefunctions to the built engine directory
    file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/Debug/UpdateFunctions)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/UpdateFunctions/Default/ ${CMAKE_CURRENT_BINARY_DIR}/Debug/UpdateFunctions/Default)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/UpdateFunctions/Custom/ ${CMAKE_CURRENT_BINARY_DIR}/Debug/UpdateFunctions/Custom)

    #move all the BuildCode to the built engine directory
    file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/Debug/BuildCode)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/BuildCode/ ${CMAKE_CURRENT_BINARY_DIR}/Debug/BuildCode)
endif()

include(CPack)
