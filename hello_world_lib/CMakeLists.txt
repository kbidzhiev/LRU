add_library(hello_world_lib)

target_sources(hello_world_lib
  PRIVATE
    hello.cpp

  PUBLIC
    FILE_SET HEADERS
    FILES
      hello.hpp
)
