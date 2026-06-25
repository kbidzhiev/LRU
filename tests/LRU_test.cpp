#include "LRU.hpp"
#include <gtest/gtest.h>
#include <vector>

namespace {
struct page_t {
  int id;
};

page_t slow_get_page(int page_id) { return page_t{page_id}; }

TEST(LRU, zero_space_cache) {
  cache::LRU_t<page_t> lru{0};
  EXPECT_TRUE(lru.full());

  EXPECT_FALSE(lru.lookup_update(1, slow_get_page));
  EXPECT_FALSE(lru.lookup_update(1, slow_get_page));
  EXPECT_TRUE(lru.full());
}

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

size_t nhits(const std::vector<int> &input) {
  size_t cache_sz = input[0];
  std::vector<int> page_ids(input.begin() + 2, input.end());

  cache::LRU_t<page_t> lru{cache_sz};
  size_t hits{};
  for (int page_id : page_ids) {
    if (lru.lookup_update(page_id, slow_get_page))
      hits += 1;
  }
  return hits;
}

struct CacheHits {
  size_t hit;
  std::vector<int> data;
};

TEST(LRU, example_from_lecture) {
  std::vector<CacheHits> input_hits = {
      {4, {2, 6, 1, 2, 1, 2, 1, 2}},
      {0, {3, 7, 1, 2, 3, 4, 5, 6, 7}},
      {6, {4, 12, 1, 2, 3, 4, 1, 2, 5, 1, 2, 4, 3, 4}}, // xxxx12x124x4 - 6 hits example from slides
  };

  for (auto &[cache_hits, input_data] : input_hits) {
    EXPECT_EQ(nhits(input_data), cache_hits);
  }
}
} // namespace
