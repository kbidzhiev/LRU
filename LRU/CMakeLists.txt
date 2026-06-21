add_library(LRU INTERFACE)

target_sources(LRU
  INTERFACE
    FILE_SET HEADERS
    FILES
    LRU.hpp
)


target_compile_options(LRU
  INTERFACE
    -Wall
    -Wextra
    -Werror
)
