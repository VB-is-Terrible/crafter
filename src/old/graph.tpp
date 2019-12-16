#include "graph.h"
#include <algorithm>
#include <initializer_list>
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <unordered_set>
#include <vector>

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<N>::const_iterator begin,
                         typename std::vector<N>::const_iterator end) {
  for (auto iter = begin; iter != end; iter++) {
    InsertNode(*iter);
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<std::tuple<N, N, E>>::const_iterator begin,
                         typename std::vector<std::tuple<N, N, E>>::const_iterator end) {
  for (auto iter = begin; iter != end; iter++) {
    auto [name1, name2, weight] = *iter;
    InsertNode(name1);
    InsertNode(name2);
    InsertEdge(name1, name2, weight);
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::initializer_list<N> list) {
  for (auto& item : list) {
    InsertNode(item);
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(const gdwg::Graph<N, E>& other) {
  nodes_ = other.nodes_;
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(gdwg::Graph<N, E>&& other) {
  nodes_ = std::move(other.nodes_);
  other.nodes_ = decltype(other.nodes_)();
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertNode(const N& val) {
  auto ptr = get_pointer(val);
  if (nodes_.count(ptr) == 1) {
    return false;
  } else {
    Nodes<N, E> node;
    node.value = ptr;
    nodes_[ptr] = std::move(node);
    // std::cout << "Inserted item, " << nodes_.size() << " items in map\n";
    return true;
  }
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertEdge(const N& src, const N& dst, const E& w) {
  auto src_key = check_pointer(src);
  auto dest_key = check_pointer(dst);
  if (src_key == NULL || dest_key == NULL) {
    throw std::runtime_error(
        "Cannot call Graph::InsertEdge when either src or dst node does not exist");
  }
  auto& src_node = nodes_.find(src_key)->second;
  auto& dest_node = nodes_.find(dest_key)->second;

  auto src_edges = src_node.edges.find(dest_key);
  if (src_edges == src_node.edges.end()) {
    auto ptr = std::make_shared<E>(w);
    src_node.edges[dest_key] = std::set<std::shared_ptr<E>, mixed_pointers_cmp<E>>{ptr};
    dest_node.incoming.insert(src_key);
    return true;
  } else {
    auto edges = src_node.edges[dest_key];
    auto edge_ptr = check_edge_ptr(edges, w);
    if (edge_ptr == NULL) {
      auto ptr = std::make_shared<E>(w);
      src_node.edges[dest_key].insert(ptr);
      return true;
    } else {
      return false;
    }
  }
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::DeleteNode(const N& value) {
  auto value_ptr = check_pointer(value);
  std::weak_ptr<N> tester = value_ptr;

  if (value_ptr == NULL) {
    return false;
  } else {
    auto& node = nodes_[value_ptr];
    // Erase inbound connections to the node
    for (auto inbound : node.incoming) {
      release(nodes_[inbound], node);
    }
    // Erase outbound connections
    for (auto outbound : node.edges) {
      auto& outbound_node = nodes_[outbound.first];
      auto result = outbound_node.incoming.erase(value_ptr);
      if (result == 0) {
        std::cerr << "Failed to remove incoming pointer";
      }
    }
    nodes_.erase(value_ptr);
    value_ptr.reset();
  }
  // std::cout << tester.use_count() << " users \n";
  // Let the destructor do the rest
  return true;
}

template <typename N, typename E>
void gdwg::Graph<N, E>::release(Nodes<N, E>& src, Nodes<N, E>& dest) {
  if (src.edges.find(dest.value) == src.edges.end()) {
    std::cerr << "Couldn't release node for " << *(dest.value) << "\n";
  }
  // std::cerr << "hi\n";
  src.edges.erase(dest.value);
}

template <typename N, typename E>
std::shared_ptr<N> gdwg::Graph<N, E>::get_pointer(const N& value) {
  auto ptr = check_pointer(value);
  if (ptr == NULL) {
    auto new_ptr = std::make_shared<N>(value);
    return new_ptr;
  } else {
    return ptr;
  }
}

template <typename N, typename E>
std::shared_ptr<N> gdwg::Graph<N, E>::check_pointer(const N& value) const {
  auto iter = std::lower_bound(nodes_.begin(), nodes_.end(), value, node_cmp<N, E>());
  if (iter == nodes_.end() || *(iter->first) != value) {
    return std::shared_ptr<N>();
  } else {
    return iter->first;
  }
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::IsNode(const N& value) const {
  auto ptr = check_pointer(value);
  if (ptr == NULL) {
    return false;
  } else {
    return true;
  }
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::IsConnected(const N& src, const N& dst) const {
  auto src_ptr = check_pointer(src);
  auto dst_ptr = check_pointer(dst);
  if (src_ptr == NULL || dst_ptr == NULL) {
    throw std::runtime_error(
        "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
  }
  const auto& src_node = nodes_.find(src_ptr)->second;
  if (src_node.edges.find(dst_ptr) == src_node.edges.end()) {
    return false;
  } else {
    return true;
  }
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetNodes() const {
  std::vector<N> result(nodes_.size());
  auto count = 0;
  for (auto iter : nodes_) {
    result[count] = *iter.first;
    count++;
  }
  return result;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetConnected(const N& value) const {
  auto ptr = check_pointer(value);
  if (ptr == NULL) {
    throw std::out_of_range("Cannot call Graph::GetConnected if src doesn't exist in the graph");
  }
  const auto& node = nodes_.find(ptr)->second;
  std::vector<N> result;
  for (auto pair : node.edges) {
    result.push_back(*pair.first);
  }
  return result;
}

template <typename N, typename E>
std::vector<E> gdwg::Graph<N, E>::GetWeights(const N& src, const N& dst) const {
  auto src_ptr = check_pointer(src), dst_ptr = check_pointer(dst);
  if (src_ptr == NULL || dst_ptr == NULL) {
    throw std::out_of_range(
        "Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
  }
  auto& src_node = nodes_.find(src_ptr)->second;
  std::vector<E> result;
  if (src_node.edges.count(dst_ptr)) {
    auto edges = src_node.edges.find(dst_ptr)->second;
    for (auto edge : edges) {
      result.push_back(*edge);
    }
  }
  return result;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::erase(const N& src, const N& dst, const E& w) {
  auto src_ptr = check_pointer(src), dst_ptr = check_pointer(dst);
  if (src_ptr == NULL || dst_ptr == NULL) {
    return false;
  }
  auto& src_node = nodes_.find(src_ptr)->second;
  auto iter = src_node.edges.find(dst_ptr);
  if (iter == src_node.edges.end()) {
    return false;
  }
  auto& weights = src_node.edges[dst_ptr];
  auto edge_ptr = check_edge_ptr(weights, w);
  if (edge_ptr == NULL) {
    return false;
  }
  weights.erase(edge_ptr);
  if (weights.size() == 0) {
    src_node.edges.erase(dst_ptr);
  }
  return true;
}

template <typename N, typename E>
std::shared_ptr<E>
gdwg::Graph<N, E>::check_edge_ptr(const std::set<std::shared_ptr<E>, mixed_pointers_cmp<E>> edges,
                                  const E& w) const {
  auto iter = std::lower_bound(edges.begin(), edges.end(), w, mixed_pointers_cmp<E>());
  if (iter == edges.end() || **iter != w) {
    return std::shared_ptr<E>();
  } else {
    return *iter;
  }
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::Replace(const N& oldData, const N& newData) {
  auto old_ptr = check_pointer(oldData);
  if (old_ptr == NULL) {
    throw std::runtime_error("Cannot call Graph::Replace on a node that doesn't exist");
  }
  auto new_ptr = check_pointer(newData);
  if (new_ptr != NULL) {
    return false;
  }
  InsertNode(newData);
  // Now insert all the edges

  auto& old_node = nodes_[old_ptr];
  for (auto outbound : old_node.edges) {
    auto weights = outbound.second;
    for (auto weight : weights) {
      std::shared_ptr<N> dst_ptr;
      if (outbound.first == old_ptr) {
        InsertEdge(newData, newData, *weight);
      } else {
        InsertEdge(newData, *outbound.first, *weight);
      }
    }
  }

  for (auto incoming : old_node.incoming) {
    auto weights = GetWeights(*incoming, oldData);
    for (auto weight : weights) {
      InsertEdge(*incoming, newData, weight);
    }
  }

  DeleteNode(oldData);
  return true;
}

template <typename N, typename E>
void gdwg::Graph<N, E>::MergeReplace(const N& oldData, const N& newData) {
  auto old_ptr = check_pointer(oldData), new_ptr = check_pointer(newData);
  if (old_ptr == NULL || new_ptr == NULL) {
    throw std::runtime_error(
        "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
  }
  auto& old_node = nodes_[old_ptr];

  for (auto outbound : old_node.edges) {
    auto weights = outbound.second;
    for (auto weight : weights) {
      std::shared_ptr<N> dst_ptr;
      if (outbound.first == old_ptr) {
        InsertEdge(newData, newData, *weight);
      } else {
        InsertEdge(newData, *outbound.first, *weight);
      }
    }
  }

  for (auto incoming : old_node.incoming) {
    auto weights = GetWeights(*incoming, oldData);
    for (auto weight : weights) {
      InsertEdge(*incoming, newData, weight);
    }
  }

  DeleteNode(oldData);
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cbegin() const {
  auto src_iter = nodes_.cbegin();

  dst_iter dst_iter;
  edge_iter edge_iter;

  if (src_iter == nodes_.cend()) {
    return this->cend();
  }
  dst_iter = src_iter->second.edges.cbegin();
  // Check for EOL and if node has edges
  while (src_iter != nodes_.cend() && dst_iter == src_iter->second.edges.cend()) {
    ++src_iter;
    dst_iter = src_iter->second.edges.cbegin();
  }
  if (src_iter == nodes_.end()) {
    return const_iterator{nodes_.cend(), nodes_.cend(), empty_edge_map.cend(),
                          empty_edge_set.cend()};
  } else {
    edge_iter = dst_iter->second.cbegin();
    return const_iterator{nodes_.cend(), src_iter, dst_iter, edge_iter};
  }
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cend() const {
  // auto src_iter = nodes_.cend();

  // Check for empty graph
  return const_iterator{nodes_.cend(), nodes_.cend(), empty_edge_map.cend(), empty_edge_set.cend()};
  // Check for EOL and if node has edges
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::begin() const {
  return this->cbegin();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::end() const {
  return this->cend();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator
gdwg::Graph<N, E>::find(const N& src, const N& dst, const E& w) const {
  auto src_ptr = check_pointer(src), dst_ptr = check_pointer(dst);
  if (src_ptr == NULL || dst_ptr == NULL) {
    return this->cend();
  }
  const auto src_iter = nodes_.find(src_ptr);
  const auto& node = src_iter->second;
  const auto dst_iter = node.edges.find(dst_ptr);
  if (dst_iter == node.edges.end()) {
    return this->cend();
  }
  const auto& set = dst_iter->second;
  auto edge_ptr = check_edge_ptr(set, w);
  if (edge_ptr == NULL) {
    return this->cend();
  }
  const auto edge_iter = set.find(edge_ptr);
  return const_iterator{nodes_.end(), src_iter, dst_iter, edge_iter};
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::erase(const_iterator it) {
  // Check for end
  if (it == this->cend()) {
    // Just skip
    return this->cend();
  }
  // Values for iterators should now be valid
  const auto& [src, dst, w] = *it;

  // Get the values for the next iterator

  ++it;

  if (it == this->cend()) {
    this->erase(src, dst, w);
    return this->cend();
  } else {
    const auto& [src2, dst2, w2] = *it;
    this->erase(src, dst, w);
    return this->find(src2, dst2, w2);
  }
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator gdwg::Graph<N, E>::crbegin() {
  return const_reverse_iterator{cend()};
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator gdwg::Graph<N, E>::rbegin() {
  return crbegin();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator gdwg::Graph<N, E>::rend() {
  return crend();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator gdwg::Graph<N, E>::crend() {
  return const_reverse_iterator{cbegin()};
}
