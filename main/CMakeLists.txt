add_executable(main)
target_sources(main
  PRIVATE
    main.cpp
)

target_compile_options(main PRIVATE
    -Wall
    -Wextra
    -Werror
)


target_link_libraries(main
  PRIVATE
    LRU 
)
