add_library(LRU INTERFACE)

target_sources(LRU
  INTERFACE
    FILE_SET HEADERS
    FILES
    LRU.hpp
)
