#include "LRU.hpp"
#include <cassert>
#include <iostream>
#include <map>
#include <string>

int slow_get_page(int key) { return key; }


int main() {
  std::map<std::string, int> cache_hits;
  size_t sz;
  int nelts;

  std::cin >> sz >> nelts;

  cache::LRU_t<int> lru{sz};

  for (int i = 0; i < nelts; ++i) {
    int p;
    std::cin >> p;
    assert(std::cin.good());
    if (lru.lookup_update(p, slow_get_page))
      cache_hits["LRU"] += 1;
  }

  std::cout << "Cache type\tN hits\n";
  for(auto [cache_type, nhits]: cache_hits){
    std::cout << cache_type << "\t" << nhits << std::endl;
  }
  return 0;
}
