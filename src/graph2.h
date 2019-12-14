#pragma once

#include <map>
#include <memory>
#include <string>
#include <set>
#include <unordered_set>
#include <vector>
#include <iostream>



namespace graph {

template <typename N, typename E>
struct Nodes;

template <typename N, typename E>
using edge_map = std::map<N, E> ;

template <typename N, typename E>
using node_map = std::map<N, Nodes<N, E>>;

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
bool operator==(const Graph<N, E>& lhs, Graph<N, E>& rhs);

template <typename N, typename E>
class Graph {
private:
	node_map<N, E> nodes;

	static bool node_check(const Graph<N, E>& lhs, const Graph<N, E>& rhs);
	static bool edge_check(const Graph<N, E>& lhs, const Graph<N, E>& rhs);
public:
	Graph(typename std::vector<N>::const_iterator, typename std::vector<N>::const_iterator);

        Graph<N, E>(typename std::vector<std::tuple<N, N, E>>::const_iterator,
                    typename std::vector<std::tuple<N, N, E>>::const_iterator);

        Graph<N, E>(std::initializer_list<N>);
        Graph<N, E>(const Graph<N, E>&);
        Graph<N, E>(Graph<N, E>&&);
        Graph() = default;
        ~Graph() = default;

	bool InsertNode(const N& val);
	bool InsertEdge(const N& src, const N& dst, const E& w);
	bool DeleteNode(const N&);

	friend std::ostream& operator<<(std::ostream& os, const Graph<N, E>& graph);

	friend bool operator==(const Graph<N, E>& lhs, Graph<N, E>& rhs);

};


}