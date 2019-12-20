#pragma once

#ifndef CRAFTER_GRAPH
#define CRAFTER_GRAPH

#include <initializer_list>
#include <unordered_map>
#include <memory>
#include <string>
#include <set>
#include <unordered_set>
#include <vector>
#include <iterator>
#include <iostream>

namespace graph {

template <typename N, typename E>
struct Nodes;

template <typename N, typename E>
using edge_map = std::unordered_map<N, E> ;

template <typename N, typename E>
using node_map = std::unordered_map<N, Nodes<N, E>>;

template <typename N, typename E>
struct Nodes {
	edge_map<N, E> edges;
	N value;
	std::unordered_set<N> incoming;
	bool operator<(const struct Nodes& other) { return value < other.value; }
};

template <typename N, typename E>
class Graph;

template <typename N, typename E>
std::ostream& operator<<(std::ostream& os, const Graph<N, E>& graph);

template <typename N, typename E>
bool operator==(const Graph<N, E>& lhs, const Graph<N, E>& rhs);

template <typename N, typename E>
class _const_iterator;

template <typename N, typename E>
bool operator==(const _const_iterator<N, E>& lhs, const _const_iterator<N, E>& rhs);

template <typename N, typename E>
bool operator!=(const _const_iterator<N, E>& lhs, const _const_iterator<N, E>& rhs);

template <typename N, typename E>
class Graph {
private:
	node_map<N, E> nodes;

	friend std::ostream& operator<< <N, E>(std::ostream& os, const Graph<N, E>&);
	friend bool operator== <N, E>(const Graph<N, E>& lhs, const Graph<N, E>& rhs);

	static bool node_check(const Graph<N, E>& lhs, const Graph<N, E>& rhs);
	static bool edge_check(const Graph<N, E>& lhs, const Graph<N, E>& rhs);
public:
	Graph(typename std::vector<N>::const_iterator, typename std::vector<N>::const_iterator);

	Graph<N, E>(typename std::vector<std::tuple<N, N, E>>::const_iterator,
	            typename std::vector<std::tuple<N, N, E>>::const_iterator);

	Graph<N, E>(const std::initializer_list<N>);
	Graph<N, E>(const Graph<N, E>&);
	Graph<N, E>(Graph<N, E>&&);
	Graph() = default;
	~Graph() = default;

	bool InsertNode(const N& val);
	bool InsertEdge(const N& src, const N& dst, const E& w);
	bool DeleteNode(const N&);

	bool IsNode(const N&) const;
	bool IsConnected(const N& src, const N& dst) const;
	std::vector<N> GetNodes() const;
	std::vector<N> GetConnected(const N&) const;
	std::vector<N> GetIncoming(const N&) const;
	E GetWeight(const N& src, const N& dst) const;
	bool erase(const N& src, const N& dst);
	bool SetWeight(const N& src, const N& dst, const E& w);
	bool Replace(const N& oldData, const N& newData);
	void MergeReplace(const N& oldData, const N& newData);

	using const_iterator = _const_iterator<N, E>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	const_iterator cbegin() const;
	const_iterator cend() const;
	const_iterator erase(const_iterator it);
	const_iterator find(const N&) const;
	const_reverse_iterator crbegin() const;
	const_reverse_iterator crend() const;
	const_reverse_iterator rbegin() const;
        const_reverse_iterator rend() const;
        const_iterator begin() const;
        const_iterator end() const;
};


template <typename N, typename E>
class _const_iterator {
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = N;
	using pointer = N*;
	using reference = const N&;
	using difference_type = int;
public:
	reference operator*() const;
	_const_iterator operator++();
	_const_iterator operator++(int);
	_const_iterator operator--();
	_const_iterator operator--(int);
	friend bool operator== <N ,E>(const _const_iterator& lhs, const _const_iterator& rhs);
	friend bool operator!= <N, E>(const _const_iterator& lhs, const _const_iterator& rhs);
private:
	typename node_map<N, E>::const_iterator iter_;
	explicit _const_iterator(typename node_map<N, E>::const_iterator iter) : iter_ {iter} {};
	friend class Graph<N, E>;

};

}

#include "graph.tpp"

#endif /* end of include guard: graph2 */
