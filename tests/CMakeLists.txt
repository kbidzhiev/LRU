include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE # updates file stamps on extraction
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(caches_test
  LRU_test.cpp
  LFU_test.cpp
)

target_compile_options(caches_test
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Werror>
)

target_link_libraries(caches_test
  PRIVATE
    LRU
    LFU
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(caches_test)
