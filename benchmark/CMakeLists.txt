add_executable(benchmark)
target_sources(benchmark
  PRIVATE
    benchmark.cpp
)

target_compile_options(benchmark
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Werror>
)

target_link_libraries(benchmark
  PRIVATE
    LRU 
    LFU
    perfect
)
