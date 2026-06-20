cmake_minimum_required(VERSION 3.23)

project(cache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(hello_world_lib)
add_subdirectory(main)


include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()


