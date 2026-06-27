#pragma once

#include <algorithm>
#include <cassert>
#include <list>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cache {

template <typename T, typename KeyT = int> class perfect_t {
  size_t sz_;
  using KV = typename std::pair<KeyT, T>;

  using const_MapIt = typename std::unordered_map<KeyT, T>::const_iterator;
  std::unordered_map<KeyT, T> hash_;
  std::list<KeyT> future_keys_;

public:
  explicit perfect_t(size_t sz, const std::vector<KeyT> &fkeys)
      : sz_{sz}, future_keys_(fkeys.begin(), fkeys.end()) {}

  bool full() const { return hash_.size() == sz_; };

  template <typename F> bool lookup_update(KeyT key, F slow_get_page) {
    if (sz_ == 0) {
      return false;
    }

    assert(future_keys_.front() == key);
    future_keys_.pop_front();

    if (hash_.find(key) != hash_.end())
      return true;

    // not found
    if (full()) {
      // eviction strategy:
      // "Replace the page that will not be used for the longest time in the
      // future."
      auto it_to_remove = find_furthest_in_the_future();
      hash_.erase(it_to_remove);
    }
    hash_.emplace(key, slow_get_page(key));
    return false;
  }

private:
  size_t distance_until(const KeyT &key) const {
    auto it = std::find(future_keys_.begin(), future_keys_.end(), key);
    return std::distance(future_keys_.begin(), it);
  }

  const_MapIt find_furthest_in_the_future() const {
    return std::max_element(
        hash_.begin(), hash_.end(), [&](const KV &lhs, const KV &rhs) {
          return distance_until(lhs.first) < distance_until(rhs.first);
        });
  }
};
} // namespace cache
