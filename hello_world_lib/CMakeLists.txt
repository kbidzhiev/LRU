add_library(Hello_world_lib)

target_sources(Hello_world_lib
  PRIVATE
    hello.cpp

  PUBLIC
    FILE_SET HEADERS
    FILES
      hello.hpp
)
