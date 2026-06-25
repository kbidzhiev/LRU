#include "perfect.hpp"
#include <cassert>
#include <gtest/gtest.h>
#include <vector>

namespace {
struct page_t {
  int id;
};

page_t slow_get_page(int page_id) { return page_t{page_id}; }

TEST(Perfect_cache, zero_space_cache) {
  cache::perfect_t<page_t> perf_cache{0, {1, 1}};
  EXPECT_TRUE(perf_cache.full());

  EXPECT_FALSE(perf_cache.lookup_update(1, slow_get_page));
  EXPECT_FALSE(perf_cache.lookup_update(1, slow_get_page));
  EXPECT_TRUE(perf_cache.full());
}

TEST(Perfect_cache, one_hit) {
  cache::perfect_t<page_t> perf_cache{1, {1, 1, 1}};
  EXPECT_FALSE(perf_cache.full());

  EXPECT_FALSE(
      perf_cache.lookup_update(1, slow_get_page)); // cache is empty, no hits
  EXPECT_TRUE(perf_cache.full());
  EXPECT_TRUE(perf_cache.lookup_update(1, slow_get_page)); // cache hit
  EXPECT_TRUE(perf_cache.lookup_update(1, slow_get_page)); // cache hit
  EXPECT_TRUE(perf_cache.full());
}

TEST(Perfect_cache, is_full) {
  cache::perfect_t<page_t> perf_cache{2, {1, 2}};

  perf_cache.lookup_update(1, slow_get_page);
  EXPECT_FALSE(perf_cache.full());

  perf_cache.lookup_update(2, slow_get_page);
  EXPECT_TRUE(perf_cache.full());
}

TEST(Perfect_cache, no_eviction) {
  cache::perfect_t<page_t> perf_cache{3, {1, 2, 1, 3, 1, 2}};

  EXPECT_FALSE(perf_cache.lookup_update(1, slow_get_page));
  EXPECT_FALSE(perf_cache.lookup_update(2, slow_get_page));
  EXPECT_TRUE(perf_cache.lookup_update(1, slow_get_page));
  EXPECT_FALSE(perf_cache.lookup_update(3, slow_get_page));
  EXPECT_TRUE(perf_cache.lookup_update(1, slow_get_page));
  EXPECT_TRUE(perf_cache.lookup_update(2, slow_get_page));
}

TEST(Perfect_cache, eviction) {
  // perfect cache knows the future
  cache::perfect_t<page_t> perf_cache{2, {1, 1, 2, 3, 2}};

  perf_cache.lookup_update(1, slow_get_page); // [1] future: 1 2 3 2
  perf_cache.lookup_update(1, slow_get_page); // [1] future: 2 3 2
  perf_cache.lookup_update(2, slow_get_page); // [1, 2] future: 3, 2
  perf_cache.lookup_update(3,
                           slow_get_page); // [2, 3] no future for 1, eviction

  EXPECT_TRUE(perf_cache.lookup_update(2, slow_get_page));
}

TEST(Perfect_cache, eviction_2) {
  // perfect cache knows the future
  cache::perfect_t<page_t> perf_cache{2, {1, 2, 1, 2, 3, 1, 3, 2}};

  perf_cache.lookup_update(1, slow_get_page); // [1] future 2, 1, 2, 3, 1, 3, 2
  perf_cache.lookup_update(2,
                           slow_get_page);    // [1,2] future 1, 2, 3, 1, 3, 2
  perf_cache.lookup_update(1, slow_get_page); // [1, 2] future 2, 3, 1, 3, 2
  perf_cache.lookup_update(2, slow_get_page); // [1, 2] future 3, 1, 3, 2

  perf_cache.lookup_update(3, slow_get_page); // furthest in the future is 2, so
                                              // evict it; [1, 3] future 1, 3, 2

  EXPECT_TRUE(perf_cache.lookup_update(1, slow_get_page)); // [1, 3] future 3, 2
  EXPECT_TRUE(perf_cache.lookup_update(3, slow_get_page)); // [1, 3] future 2
  EXPECT_FALSE(perf_cache.lookup_update(
      2, slow_get_page)); // doesn't matter which evict, both has no future
}

size_t nhits(const std::vector<int> &input) {
  size_t cache_sz = input[0];
  std::vector<int> page_ids(input.begin() + 2, input.end());

  cache::perfect_t<page_t> perf_cache{cache_sz, page_ids};
  size_t hits{};
  for (int page_id : page_ids) {
    if (perf_cache.lookup_update(page_id, slow_get_page))
      hits += 1;
  }
  return hits;
}

struct CacheHits {
  size_t hit;
  std::vector<int> data;
};

TEST(Perfect_cache, end_to_end) {
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
      {6, {4, 12, 1, 2, 3, 4, 1, 2, 1, 5, 1, 2, 3, 4}}, // xxxx121x123 - 6 hits
      {7, {3, 11, 1, 2, 3, 1, 1, 2, 1, 4, 1, 2, 1}},    // xxx1121x121 - 7 hits
      {3, {1, 5, 1, 1, 2, 2, 2}},                       // x1x22 - 3 hits
      {9, {3, 12, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3}}  // xxx123123123 - 9 hits
  };

  for (auto &[cache_hits, input_data] : input_hits) {
    EXPECT_EQ(nhits(input_data), cache_hits);
  }
}
} // namespace
