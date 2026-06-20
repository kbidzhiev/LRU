#include "hello.hpp"
#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect equality.
  EXPECT_EQ(hello(), std::string("Hello, world!"));
}
