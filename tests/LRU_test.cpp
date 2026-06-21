#include "LRU.hpp"
#include <gtest/gtest.h>

namespace {
struct page_t {
  int id;
};

page_t slow_get_page(int page_id) { return page_t{page_id}; }
} // namespace

TEST(LRU, no_hit) {
  cache::LRU_t<page_t> lru{3};

  // cache is empty, no hits
  EXPECT_FALSE(lru.lookup_update(1, slow_get_page));
}

TEST(LRU, one_hit) {
  cache::LRU_t<page_t> lru{3};

  EXPECT_FALSE(lru.lookup_update(1, slow_get_page));
  EXPECT_TRUE(lru.lookup_update(1, slow_get_page)); // cache hit
}

TEST(LRU, is_full) {
  cache::LRU_t<page_t> lru{2};

  lru.lookup_update(1, slow_get_page); // [1]
  lru.lookup_update(1, slow_get_page); // [1]
  EXPECT_FALSE(lru.full());

  lru.lookup_update(2, slow_get_page); // [2, 1]
  EXPECT_TRUE(lru.full());
}

TEST(LRU, no_eviction) {
  cache::LRU_t<page_t> lru{3};

  EXPECT_FALSE(lru.lookup_update(1, slow_get_page)); // [1]
  EXPECT_FALSE(lru.lookup_update(2, slow_get_page)); // [2, 1]
  EXPECT_TRUE(lru.lookup_update(1, slow_get_page));  // [1, 2]
  EXPECT_FALSE(lru.lookup_update(3, slow_get_page)); // [3, 1, 2]
  EXPECT_TRUE(lru.lookup_update(1, slow_get_page));  // [1, 3, 2]
  EXPECT_TRUE(lru.lookup_update(2, slow_get_page));  // [2, 1, 3]
}

TEST(LRU, eviction) {
  cache::LRU_t<page_t> lru{2};

  lru.lookup_update(1, slow_get_page); // [1]
  lru.lookup_update(2, slow_get_page); // [2, 1]
  lru.lookup_update(3, slow_get_page); // [3, 2]

  EXPECT_FALSE(
      lru.lookup_update(1, slow_get_page)); // 1 is not present before lookup
}

TEST(LRU, example_from_lecture) {
  cache::LRU_t<page_t> lru{2};

  size_t hits{};
  for (int page_id : {1, 2, 1, 2, 1, 2}) {
    if (lru.lookup_update(page_id, slow_get_page))
      hits += 1;
  }

  EXPECT_EQ(4, hits);
}
