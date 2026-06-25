#include "LFU.hpp"
#include "LRU.hpp"
#include "perfect.hpp"
#include <cassert>
#include <iostream>
#include <map>
#include <string>

int slow_get_page(int key) { return key; }

int main() {
  size_t sz;
  int nelts;

  std::cin >> sz >> nelts;
  std::vector<int> keys;
  for (int i = 0; i < nelts; ++i) {
    int key;
    std::cin >> key;
    assert(std::cin.good());
    keys.push_back(key);
  }

  cache::perfect_t<int> perfect_cache(sz, keys);
  cache::LRU_t<int> lru{sz};
  cache::LFU_t<int> lfu{sz};
  std::map<std::string, int> cache_hits;
  for (const auto key : keys) {
    if (perfect_cache.lookup_update(key, slow_get_page))
      cache_hits["Belady"] += 1;

    if (lru.lookup_update(key, slow_get_page))
      cache_hits["LRU"] += 1;

    if (lfu.lookup_update(key, slow_get_page))
      cache_hits["LFU"] += 1;
  }

  std::cout << "Cache type\tN hits\n";
  for (auto [cache_type, nhits] : cache_hits) {
    std::cout << cache_type << "\t" << nhits << std::endl;
  }
  return 0;
}
