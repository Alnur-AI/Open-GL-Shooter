if (WIN32)
    set(CMAKE_VER 3.10)
else()
    set(CMAKE_VER 3.10)
endif()

cmake_minimum_required(VERSION ${CMAKE_VER})

set(ADDITIONAL_LIBS "")
set(PROJECT_NAME Chess)
if(WIN32)
    set(CMAKE_CXX_COMPILER "C:/Program Files/LLVM/bin/clang++.exe")
    set(CMAKE_C_COMPILER "C:/Program Files/LLVM/bin/clang.exe")
    set(ADDITIONAL_LIBS winmm.lib Version.lib Setupapi.lib)
else()
    set(ADDITIONAL_LIBS -ldl)
endif()
project(${PROJECT_NAME})


set(PARENT_DIR ${CMAKE_SOURCE_DIR}/..)
set(PROJECT_ROOT ${CMAKE_SOURCE_DIR}/Sources)

if(WIN32)
    set (BuildDir "${PARENT_DIR}/Builds")
    add_library(SDL2 STATIC IMPORTED)
    set_target_properties(SDL2 PROPERTIES
        IMPORTED_LOCATION_RELEASE "${BuildDir}/win/libSDL2.a"
        IMPORTED_LOCATION_DEBUG "${BuildDir}/win/libSDL2.a"
        LINKER_LANGUAGE CXX)
else()
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
endif()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-pragma-pack -Wno-deprecated-declarations")


if(BUILD_TYPE STREQUAL "dbg")
    set(CMAKE_BUILD_TYPE "Debug")
elseif(BUILD_TYPE STREQUAL "rel")
    set(CMAKE_BUILD_TYPE  "Release")
endif()

message(${PROJECT_ROOT})

file(GLOB_RECURSE ENGINE_SOURCES ${PROJECT_ROOT}/*.cpp)
file(GLOB_RECURSE ENGINE_C_SOURCES ${PROJECT_ROOT}/*.c)

include_directories(${PROJECT_ROOT})

add_executable(${PROJECT_NAME} ${ENGINE_SOURCES} ${ENGINE_C_SOURCES})
target_link_libraries(${PROJECT_NAME} SDL2 ${ADDITIONAL_LIBS})




