#include "LRU.hpp"
#include <gtest/gtest.h>



int slow_get_page(int key) {return key;}


// Demonstrate some basic assertions.
TEST(LRU, ctr) {
  cache::LRU_t<int> lru{3};

  // cache is empty, no hits
  EXPECT_FALSE(lru.lookup_update(1, slow_get_page));
  EXPECT_FALSE(lru.lookup_update(2, slow_get_page));
  EXPECT_FALSE(lru.lookup_update(3, slow_get_page));
}
