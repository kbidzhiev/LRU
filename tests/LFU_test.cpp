#include "LFU.hpp"
#include <gtest/gtest.h>
#include <vector>

namespace {
struct page_t {
  int id;
};

page_t slow_get_page(int page_id) { return page_t{page_id}; }

TEST(LFU, zero_space_cache) {
  cache::LFU_t<page_t> lfu{0};
  EXPECT_TRUE(lfu.full());

  EXPECT_FALSE(lfu.lookup_update(1, slow_get_page));
  EXPECT_FALSE(lfu.lookup_update(1, slow_get_page));
  EXPECT_TRUE(lfu.full());
}

TEST(LFU, one_hit) {
  cache::LFU_t<page_t> lfu{1};
  EXPECT_FALSE(lfu.full());

  EXPECT_FALSE(lfu.lookup_update(1, slow_get_page)); // cache is empty, no hits
  EXPECT_TRUE(lfu.full());
  EXPECT_TRUE(lfu.lookup_update(1, slow_get_page)); // cache hit
  EXPECT_TRUE(lfu.lookup_update(1, slow_get_page)); // cache hit
  EXPECT_TRUE(lfu.full());
}

TEST(LFU, is_full) {
  cache::LFU_t<page_t> lfu{2};

  lfu.lookup_update(1, slow_get_page); // [{1,1}], {page, freq}
  EXPECT_FALSE(lfu.full());

  lfu.lookup_update(2, slow_get_page); // [{1,1}, {2,1}]
  EXPECT_TRUE(lfu.full());
}

TEST(LFU, no_eviction) {
  cache::LFU_t<page_t> lfu{3};

  EXPECT_FALSE(lfu.lookup_update(1, slow_get_page)); // [{1,1}] {page, freq}
  EXPECT_FALSE(lfu.lookup_update(2, slow_get_page)); // [{2,1}, {1,1}]
  EXPECT_TRUE(lfu.lookup_update(1, slow_get_page));  // [{1,2}, {2,1}]
  EXPECT_FALSE(lfu.lookup_update(3, slow_get_page)); // [{1,2}, {3,1}, {2,1}]
  EXPECT_TRUE(lfu.lookup_update(1, slow_get_page));  // [{1,3}, {3,1}, {2,1}]
  EXPECT_TRUE(lfu.lookup_update(2, slow_get_page));  // [{1,3}, {2,2}, {3,1}]
}

TEST(LFU, eviction) {
  cache::LFU_t<page_t> lfu{2};

  lfu.lookup_update(1, slow_get_page); // [{1,1}] {page, freq}
  lfu.lookup_update(1, slow_get_page); // [{1,2}]
  lfu.lookup_update(2, slow_get_page); // [{1,2}, {2,1}]
  lfu.lookup_update(3, slow_get_page); // [{1,2}, {3,1}] // 3 evicts 2

  EXPECT_FALSE(lfu.lookup_update(
      2, slow_get_page)); // 2 is not present before lookup, node 3 is evicted
}

TEST(LFU, eviction_LRU) {
  cache::LFU_t<page_t> lfu{2};

  lfu.lookup_update(1, slow_get_page); // [{1,1}] {page, freq}
  lfu.lookup_update(2,
                    slow_get_page); // [{2,1}, {1,1}] // freq are the same, but
                                    // 2 is recently used, though moves forward
  lfu.lookup_update(
      1, slow_get_page); // [{1,2}, {2,1}] // 1 is moved forward for higher freq
  lfu.lookup_update(2,
                    slow_get_page); // [{2,2}, {1,2}] // freq are the same, but
                                    // 2 is recently used, though moves forward

  // adding new page, page 1 should be evicted
  lfu.lookup_update(3, slow_get_page); // [{2,2}, {3,1}]

  EXPECT_FALSE(lfu.lookup_update(
      1, slow_get_page)); // 1 was not in LFU cache, 3 is evicted
  EXPECT_FALSE(lfu.lookup_update(
      3, slow_get_page)); // 3 was not in LFU cache, 1 is evicted
  EXPECT_TRUE(lfu.lookup_update(2, slow_get_page)); // 2 is still there
}

size_t nhits(const std::vector<int> &input) {
  size_t cache_sz = input[0];
  std::vector<int> page_ids(input.begin() + 2, input.end());

  cache::LFU_t<page_t> lfu{cache_sz};
  size_t hits{};
  for (int page_id : page_ids) {
    if (lfu.lookup_update(page_id, slow_get_page))
      hits += 1;
  }
  return hits;
}

struct CacheHits {
  size_t hit;
  std::vector<int> data;
};

TEST(LFU, example_from_lecture) {
  // in the vector the first element is cache size, second is legth of input,
  // the rest is input
  std::vector<CacheHits> input_hits = {
      {0, {0, 3, 1, 2, 3}},                             // cache size 0 - 0 hits
      {4, {2, 6, 1, 2, 1, 2, 1, 2}},                    // xx1212 - 4 hits
      {0, {3, 7, 1, 2, 3, 4, 5, 6, 7}},                 // no repeats
      {6, {4, 12, 1, 2, 3, 4, 1, 2, 5, 1, 2, 4, 3, 4}}, // xxxx12x124x4 - 6 hits
      {2, {2, 6, 1, 2, 1, 3, 1, 2}},                    // xx1x1x - 2 hits
      {4, {3, 9, 1, 2, 3, 1, 2, 4, 1, 2, 3}},           // xxx12x12x - 4 hits
      {7, {3, 12, 1, 2, 3, 1, 1, 2, 4, 1, 2, 5, 1, 2}}, // xxx112x12x12 - 7 hits
      {5, {4, 12, 1, 2, 3, 4, 1, 2, 1, 5, 1, 2, 3, 4}}, // xxxx121x123x - 5 hits
      {7, {3, 11, 1, 2, 3, 1, 1, 2, 1, 4, 1, 2, 1}},    // xxx1121x121 - 7 hits
      {3, {1, 5, 1, 1, 2, 2, 2}},                       // x1x22 - 3 hits
      {9, {3, 12, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3}}, // xxx123123123 - 9 hits
  };

  for (auto &[cache_hits, input_data] : input_hits) {
    EXPECT_EQ(nhits(input_data), cache_hits);
  }
}

} // namespace
