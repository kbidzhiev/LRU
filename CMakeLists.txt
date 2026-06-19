cmake_minimum_required(VERSION 3.23)

project(hello_world_proj)


add_executable(hello)
target_sources(hello
  PRIVATE
    src/main.cpp
)


add_library(hello_lib)
target_sources(hello_lib
  PRIVATE
    src/hw.cpp

  PUBLIC
    FILE_SET myHeaders
    TYPE HEADERS
    BASE_DIRS
      include
    FILES
      include/hw.hpp
)


target_link_libraries(hello
  PRIVATE
    hello_lib
)
