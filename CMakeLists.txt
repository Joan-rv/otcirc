cmake_minimum_required(VERSION 3.10)
project(otcirc)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

if(EMSCRIPTEN)
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} --preload-file shaders -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1"
  )
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(BUILD_FOR_WIN TRUE)
endif()
option(STATIC_BUILD "Build a static binary." ${BUILD_FOR_WIN})

if(STATIC_BUILD)
  set(CMAKE_EXE_LINKER_FLAGS "-static")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" CONFIG)
  set(BUILD_SHARED_LIBS OFF)
endif()

add_executable(otcirc src/main.cpp)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
  GIT_TAG "master"
  GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(raylib)

target_link_libraries(otcirc raylib)

if(MSVC)
  target_compile_options(otcirc PRIVATE /W4)
else()
  target_compile_options(otcirc PRIVATE -Wall -Wextra -Wpedantic
                                        -Wno-missing-field-initializers)
endif()
