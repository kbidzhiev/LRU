add_library(LFU INTERFACE)

target_sources(LFU
  INTERFACE
    FILE_SET HEADERS
    FILES
    LFU.hpp
)
