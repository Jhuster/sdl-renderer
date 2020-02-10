cmake_minimum_required(VERSION 3.2)
project(sdlrenderer)

add_definitions("-std=c++11")

set(CMAKE_CXX_STANDARD 11)

set(LIB_NAME sdlrenderer)
set(EXAMPLE_EXE example.out)

set(SDL2_INSTALL_DIR /Library/Frameworks/SDL2.framework/)

function(test_lib LIB_NAME)
    if (${LIB_NAME} MATCHES "^.*-NOTFOUND")
        message(FATAL_ERROR "lib not found: " ${LIB_NAME} " ")
        return()
    endif(${LIB_NAME} MATCHES "^.*-NOTFOUND")
endfunction(test_lib)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

find_library(SDL2_LIBS SDL2 HINTS "${SDL2_INSTALL_DIR}" STATIC)
test_lib(${SDL2_LIBS})

include_directories(src)
include_directories(${SDL2_INSTALL_DIR})

##################################### sdl-renderer lib #####################################

set(LIB_SOURCE
        src/renderer/sdl_renderer_impl.cc
        src/utils/logging.cc)

add_library(${LIB_NAME} ${LIB_SOURCE})

##################################### sdl-renderer examples #####################################
set(EXAMPLE_SOURCE
        example/main.cc)

add_executable(${EXAMPLE_EXE} ${EXAMPLE_SOURCE})

target_link_libraries(${EXAMPLE_EXE}
        ${LIB_NAME}
        ${SDL2_LIBS})
