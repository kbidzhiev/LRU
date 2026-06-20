cmake_minimum_required(VERSION 3.23)

project(cache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(main)
add_subdirectory(LRU)


include(CTest)
add_subdirectory(tests)


