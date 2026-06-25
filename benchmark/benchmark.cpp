#include "LFU.hpp"
#include "LRU.hpp"
#include "perfect.hpp"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>

int slow_get_page(int key) { return key; }

std::vector<int> generate_rnd_keys(size_t n, int mod = 100) {
  std::vector<int> keys(n);
  std::generate(keys.begin(), keys.end(),
                [&]() { return static_cast<int>(rand() % mod); });
  return keys;
}

int main() {
  size_t input_sz = 100'000;
  size_t rnd_mod = 100;
  std::vector<int> keys = generate_rnd_keys(input_sz, rnd_mod);

  std::map<int, std::map<std::string, int>> cache_sizes_vs_strategy_hits;
  std::cout << "Cache type\tN hits\n";
  for (int cache_sz = 10; cache_sz < 101; cache_sz += 10) {
    std::map<std::string, int> cache_hits;
    cache::perfect_t<int> perfect_cache(cache_sz, keys);
    cache::LRU_t<int> lru{cache_sz};
    cache::LFU_t<int> lfu{cache_sz};

    for (const auto key : keys) {
      if (perfect_cache.lookup_update(key, slow_get_page))
        cache_hits["Belady"] += 1;

      if (lru.lookup_update(key, slow_get_page))
        cache_hits["LRU"] += 1;

      if (lfu.lookup_update(key, slow_get_page))
        cache_hits["LFU"] += 1;
    }
    cache_sizes_vs_strategy_hits[cache_sz] = cache_hits;

    std::cout << "cache size: " << cache_sz << '\n';
    for (auto [cache_type, nhits] : cache_hits) {
      std::cout << cache_type << "\t" << nhits << '\n';
    }
    std::cout << std::endl;
  }

  // output to a file
  std::ofstream file("benchmark/cache_hits.dat");
  if (file.is_open()) {
    file << "#Number of keys = " << input_sz << "\n";
    file << "#input keys: uniform distribution [0:" << rnd_mod << ")\n";
    file << "#Cache_size\tLRU\tLFU\tBelady\n";
    for (auto &[sz, cache_stats] : cache_sizes_vs_strategy_hits) {
      file << sz << '\t' << cache_stats["LRU"] << '\t' << cache_stats["LFU"]
           << '\t' << cache_stats["Belady"] << '\n';
    }
    file.close();
  }

  return 0;
}
