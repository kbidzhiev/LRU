cmake_minimum_required(VERSION 3.23)

project(hello_world_proj)


add_executable(hello)

target_sources(hello
  PRIVATE
    src/main.cpp
)
