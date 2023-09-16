cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO")
cmake_policy(SET CMP0057 NEW)

project ("gdPlatformerMode")
set(creator "user95401")

#curly-eureka
include(C:\\Users\\Lenovo\\source\\curly-eureka\\CURLY-EUREKA-INCL.cmake)

#mod
add_library(${PROJECT_NAME} SHARED "main_NEW.cpp" "verInf.rc" ${curly_eureka_src})
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}")

#geode-makeup
set(GEODE_MOD_OUTPUT_PATH "D:\\Games\\user666\\Geometry.Dash.v07.24.2021\\geode\\mods")
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/geode geode-build)