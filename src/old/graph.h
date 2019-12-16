#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <initializer_list>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <tuple>
#include <unordered_set>
#include <vector>
#include <utility>

namespace gdwg {

template <typename N, typename E>
struct Nodes;

template <typename E>
struct mixed_pointers_cmp {
  bool operator()(const std::shared_ptr<E>& a, const std::shared_ptr<E>& b) const {
    return *a < *b;
  }
  bool operator()(const E& a, const std::shared_ptr<E>& b) const { return a < *b; }
  bool operator()(const std::shared_ptr<E>& a, const E& b) const { return *a < b; }
};

template <typename N, typename E>
struct Nodes;

template <typename N, typename E>
struct node_cmp : public mixed_pointers_cmp<N> {
  using mixed_pointers_cmp<N>::operator();
  bool operator()(const std::pair<const std::shared_ptr<N>, Nodes<N, E>>& a, const N& b) {
    return *(a.first) < b;
  }
  bool operator()(const N& a, const std::pair<const std::shared_ptr<N>, Nodes<N, E>>& b) {
    return *(a.first) < b;
  }
};

template <typename N, typename E>
using edge_set = std::set<std::shared_ptr<E>, mixed_pointers_cmp<E>>;

template <typename N, typename E>
using edge_map = std::map<std::shared_ptr<N>, edge_set<N, E>, node_cmp<N, E>>;

template <typename N, typename E>
using node_map = std::map<std::shared_ptr<N>, Nodes<N, E>, node_cmp<N, E>>;

template <typename N, typename E>
struct Nodes {
  edge_map<N, E> edges;
  std::shared_ptr<N> value;
  std::unordered_set<std::shared_ptr<N>> incoming;
  bool operator<(const struct Nodes& other) { return *value < *(other.value); }
  ~Nodes() {
    edges.clear();
    incoming.clear();
    value.reset();
  }
};

template <typename N, typename E>
class Graph {
  using src_iter = typename node_map<N, E>::const_iterator;
  using dst_iter = typename edge_map<N, E>::const_iterator;
  using edge_iter = typename edge_set<N, E>::const_iterator;

 public:
  class const_iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::tuple<N, N, E>;
    using pointer = std::tuple<N, N, E>*;
    using reference = const std::tuple<const N&, const N&, const E&>;
    using difference_type = int;

    // value_type operator*() const {
    //   return std::make_tuple(*(src_node_iter_->first), *(dst_node_iter_->first), **edge_iter_);
    // }
    reference operator*() const {
      return {
        *(src_node_iter_->first),
        *(dst_node_iter_->first),
        **edge_iter_
      };
    }
    const_iterator operator++() {
      iterate_edge();
      return *this;
    }
    const_iterator operator++(int) {
      auto copy{*this};
      ++(*this);
      return copy;
    }
    const_iterator operator--() {
      decrement();
      return *this;
    }
    const_iterator operator--(int) {
      auto copy{*this};
      --(*this);
      return copy;
    }
    // pointer operator-> () {return &(operator*());}

    friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) {
      if (lhs.src_node_iter_ == lhs.end_ && rhs.src_node_iter_ == rhs.end_) {
        return true;
      } else if (lhs.src_node_iter_ == lhs.end_) {
        return false;
      } else if (rhs.src_node_iter_ == rhs.end_) {
        return false;
      } else {
        return lhs.src_node_iter_ == rhs.src_node_iter_ &&
               lhs.dst_node_iter_ == rhs.dst_node_iter_ && lhs.edge_iter_ == rhs.edge_iter_;
      }
    }
    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) {
      return !(lhs == rhs);
    }

   private:
    src_iter src_node_iter_;
    dst_iter dst_node_iter_;
    edge_iter edge_iter_;
    src_iter end_;
    explicit const_iterator(src_iter end,
                            src_iter src_node_iter,
                            dst_iter dst_node_iter,
                            edge_iter edge_iter_v)
      : src_node_iter_{src_node_iter}, dst_node_iter_{dst_node_iter},
        edge_iter_{edge_iter_v}, end_{end} {}

   protected:
    bool iterate_src() {
      ++src_node_iter_;
      if (src_node_iter_ != end_) {
        dst_node_iter_ = src_node_iter_->second.edges.begin();
        return false;
      }
      return true;
    }
    bool iterate_dst() {
      ++dst_node_iter_;
      bool end = false;
      while (dst_node_iter_ == src_node_iter_->second.edges.end() && !end) {
        end = iterate_src();
      }
      if (!end) {
        edge_iter_ = dst_node_iter_->second.begin();
      }
      return end;
    }
    void iterate_edge() {
      auto& set = dst_node_iter_->second;
      // std::cout << "Iter: " << edge_iter_.get() << "    " << set.end().get() << "    " <<
      // set.begin().get();
      ++edge_iter_;
      // std::cout << "    " << edge_iter_.get() << "\n";
      if (edge_iter_ == set.cend()) {
        iterate_dst();
      }
    }
    void decrement() {
      if (src_node_iter_ == end_) {
        decrement_src();
        decrement_dst();
        decrement_edge();
      } else {
        decrement_edge();
      }
    }
    void decrement_src() {
      --src_node_iter_;
      dst_node_iter_ = src_node_iter_->second.edges.end();
    }
    void decrement_dst() {
      while (dst_node_iter_ == src_node_iter_->second.edges.begin()) {
        decrement_src();
      }
      --dst_node_iter_;
      edge_iter_ = dst_node_iter_->second.end();
    }
    void decrement_edge() {
      auto& set = dst_node_iter_->second;
      if (edge_iter_ == set.begin()) {
        decrement_dst();
      }
      --edge_iter_;
    }
    friend class Graph<N, E>;
  };

 public:
  // using const_reverse_iterator = const_iterator;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Graph(typename std::vector<N>::const_iterator, typename std::vector<N>::const_iterator);

  Graph<N, E>(typename std::vector<std::tuple<N, N, E>>::const_iterator,
              typename std::vector<std::tuple<N, N, E>>::const_iterator);

  Graph<N, E>(std::initializer_list<N>);
  Graph<N, E>(const gdwg::Graph<N, E>&);
  Graph<N, E>(gdwg::Graph<N, E>&&);
  Graph() = default;
  ~Graph() = default;

  bool InsertNode(const N& val);
  bool InsertEdge(const N& src, const N& dst, const E& w);
  bool DeleteNode(const N&);
  friend std::ostream& operator<<(std::ostream& os, const gdwg::Graph<N, E>& graph) {
    for (auto pair : graph.nodes_) {
      auto& node = pair.second;
      os << *(node.value);
      os << " (\n";
      for (auto edge_pair : node.edges) {
        auto edges = edge_pair.second;
        auto& dest = *(edge_pair.first);
        for (auto edge : edges) {
          os << "  " << dest << " | " << *edge << "\n";
        }
      }
      os << ")\n";
    }
    return os;
  }

  friend bool operator==(const gdwg::Graph<N, E>& lhs, const gdwg::Graph<N, E>& rhs) {
    // Node equality
    if (!node_check(lhs, rhs)) {
      return false;
    }
    if (!node_check(rhs, lhs)) {
      return false;
    }
    // Edge equality
    if (!edge_check(lhs, rhs)) {
      return false;
    }
    if (!edge_check(rhs, lhs)) {
      return false;
    }

    return true;
  }
  friend bool operator!=(const gdwg::Graph<N, E>& lhs, const gdwg::Graph<N, E>& rhs) {
    return !(lhs == rhs);
  }

  bool IsNode(const N&) const;
  bool IsConnected(const N& src, const N& dst) const;
  std::vector<N> GetNodes() const;
  std::vector<N> GetConnected(const N&) const;
  std::vector<E> GetWeights(const N& src, const N& dst) const;
  bool erase(const N& src, const N& dst, const E& w);
  bool Replace(const N& oldData, const N& newData);
  void MergeReplace(const N& oldData, const N& newData);

  const_iterator cbegin() const;
  const_iterator cend() const;
  const_iterator erase(const_iterator it);
  const_iterator find(const N&, const N&, const E&) const;
  const_reverse_iterator crbegin();
  const_reverse_iterator crend();
  const_reverse_iterator rbegin();
  const_reverse_iterator rend();
  const_iterator begin() const;
  const_iterator end() const;

 private:
  /*
   * Remove all edges from src to dest
   */

  friend bool node_check(const gdwg::Graph<N, E>& lhs, const gdwg::Graph<N, E>& rhs) {
    for (const auto& node : lhs.GetNodes()) {
      if (!rhs.IsNode(node)) {
        return false;
      }
    }
    return true;
  }
  friend bool edge_check(const gdwg::Graph<N, E>& lhs, const gdwg::Graph<N, E>& rhs) {
    for (const auto& src : lhs.GetNodes()) {
      for (const auto& dst : lhs.GetConnected(src)) {
        if (lhs.GetWeights(src, dst) != rhs.GetWeights(src, dst)) {
          return false;
        }
      }
    }
    return true;
  }
  void release(Nodes<N, E>&, Nodes<N, E>&);
  node_map<N, E> nodes_;
  std::shared_ptr<N> get_pointer(const N&);
  std::shared_ptr<N> check_pointer(const N&) const;
  std::shared_ptr<E> check_edge_ptr(const std::set<std::shared_ptr<E>, mixed_pointers_cmp<E>>,
                                    const E&) const;
  inline static edge_map<N, E> const empty_edge_map;
  inline static edge_set<N, E> const empty_edge_set;
};

}  // namespace gdwg

#include "graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_

// d
