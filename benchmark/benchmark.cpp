#include "LFU.hpp"
#include "LRU.hpp"
#include "perfect.hpp"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <utility>

int slow_get_page(int key) { return key; }

std::vector<int> generate_rnd_keys(size_t n, size_t mod = 100) {
  std::vector<int> keys(n);
  std::generate(keys.begin(), keys.end(), [&]() { return rand() % mod; });
  return keys;
}

int main() {
  size_t input_sz = 1'000'000;
  std::vector<int> keys = generate_rnd_keys(input_sz, 100);

  std::map<size_t, std::map<std::string, int>> cache_sizes_vs_strategy_hits;
  for (size_t cache_sz = 10; cache_sz < 101; cache_sz += 10) {
    std::map<std::string, int> cache_hits;
    cache::perfect_t<int> perfect_cache(cache_sz, keys);
    cache::LRU_t<int> lru{cache_sz};
    cache::LFU_t<int> lfu{cache_sz};

    for (const auto key : keys) {
      if (perfect_cache.lookup_update(key, slow_get_page))
        cache_hits["perfect_cache"] += 1;

      if (lru.lookup_update(key, slow_get_page))
        cache_hits["LRU"] += 1;

      if (lfu.lookup_update(key, slow_get_page))
        cache_hits["LFU"] += 1;
    }
    cache_sizes_vs_strategy_hits[cache_sz] = cache_hits;

    std::cout << "Cache type\tN hits\n";
    for (auto [cache_type, nhits] : cache_hits) {
      std::cout << cache_type << "\t" << nhits << std::endl;
    }

  }

  return 0;
}
