#pragma once

#include <algorithm>
#include <list>
#include <unordered_map>

namespace cache {

template <typename T, typename KeyT = int> class LFU_t {
  size_t sz_;
  struct NodeInfo {
    KeyT key_;
    T page_;
    size_t freq_;
  };
  std::list<NodeInfo> cache_;

  using ListIt = typename decltype(cache_)::iterator;
  std::unordered_map<KeyT, ListIt> hash_;

public:
  explicit LFU_t(size_t sz) : sz_{sz} {}

  bool full() const { return cache_.size() == sz_; };

  template <typename F> bool lookup_update(KeyT key, F slow_get_page) {
    if (sz_ == 0) {
      return false;
    }

    auto hit = hash_.find(key);
    if (hit != hash_.end()) {

      auto eltit = hit->second;
      eltit->freq_ += 1;
      auto it = find_mru_node_for_frequency(eltit->freq_);
      cache_.splice(it, cache_, eltit);
      return true;
    }

    if (full()) {
      hash_.erase(cache_.back().key_);
      cache_.pop_back();
    }
    auto it = find_mru_node_for_frequency(1);
    it = cache_.emplace(it, NodeInfo{key, slow_get_page(key), 1});
    hash_.emplace(key, it);
    return false;
  }

private:
  ListIt find_mru_node_for_frequency(size_t freq) {
    return std::lower_bound(cache_.begin(), cache_.end(), freq,
                            [](const NodeInfo &node, size_t target) {
                              return node.freq_ > target;
                            });
  }
};

} // namespace cache
