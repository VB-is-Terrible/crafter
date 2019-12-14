#include "graph2.h"

namespace graph {

template <typename N, typename E>
Graph<N, E>::Graph(typename std::vector<N>::const_iterator begin,
                   typename std::vector<N>::const_iterator end) {
	for (auto iter = begin; iter != end; iter++) {
		InsertNode(*iter);
	}
}

template <typename N, typename E>
Graph<N, E>::Graph(typename std::vector<std::tuple<N, N, E>>::const_iterator begin,
                         typename std::vector<std::tuple<N, N, E>>::const_iterator end) {
        for (auto iter = begin; iter != end; iter++) {
    		auto [name1, name2, weight] = *iter;
		InsertNode(name1);
		InsertNode(name2);
		InsertEdge(name1, name2, weight);
	}
}

template <typename N, typename E>
Graph<N, E>::Graph(typename std::initializer_list<N> list) {
	for (auto& item : list) {
		InsertNode(item);
	}
}

template <typename N, typename E>
Graph<N, E>::Graph(const Graph<N, E>& other) : nodes{other.nodes} {}

template <typename N, typename E>
Graph<N, E>::Graph(Graph<N, E>&& other) {
	nodes = std::move(other.nodes);
	other.nodes_ = decltype(other.nodes_)();
}

template <typename N, typename E>
bool Graph<N, E>::InsertNode(const N& val) {
	if (nodes.count(val) == 0) {
		nodes[val] = Nodes<N, E>();
		return true;
	} else {
		return false;
	}
}

template <typename N, typename E>
bool Graph<N, E>::InsertEdge(const N& src, const N& dst, const E& w) {
	if (!IsNode(src) || !IsNode(dst)) {
		throw std::runtime_error(
		"Cannot call Graph::InsertEdge when either src or dst node does not exist");
	}
	auto& src_node = nodes.find(src)->second;
	auto& dest_node = nodes.find(dst)->second;

	auto& src_edges = src_node.edges;
	if (src_edges.count(dst) == 0) {
		src_edges[dst] = w;
		dest_node.incoming.insert(src);
	} else {
		throw std::runtime_error(
		"Cannot call Graph::InsertEdge when the edge already exists");
	}
	return true;
}

template <typename N, typename E>
bool Graph<N, E>::DeleteNode(const N& value) {
	if (!IsNode(value)) {
		return false;
	} else {
		auto& node = nodes[value];
		for (auto& inbound : node.incoming) {
			auto& src_node = nodes[inbound];
			src_node.edges.erase(src_node);
		}
		for (auto& outbound : node.edges) {
			auto& dst_node = nodes[outbound];
			dst_node.incoming.erase(value);
		}
		nodes.erase(value);
		return true;
	}
}

template <typename N, typename E>
bool Graph<N, E>::IsNode(const N& value) const {
	auto it = nodes.find(value);
	if (it == nodes.end()) {
		return false;
	} else {
		return true;
	}
}

template <typename N, typename E>
bool Graph<N, E>::IsConnected(const N& src, const N& dst) const {
	if (!IsNode(src) || !IsNode(dst)) {
		throw std::runtime_error(
		"Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
	}
	const auto& src_node = nodes[src];
	if (src_node.edges.find(dst) == src_node.edges.end()) {
		return false;
	} else {
		return true;
	}
}

template <typename N, typename E>
std::vector<N> Graph<N, E>::GetNodes() const {
	std::vector<N> result(nodes.size());
	for (auto& iter = nodes.first(), count = 0; iter++, count++; iter = nodes.end()) {
		result[count] = *iter.first;
	}
	return result;
}

template <typename N, typename E>
std::vector<N> Graph<N, E>::GetConnected(const N& value) const {
	auto src_it = nodes.find(value);
	if (src_it == nodes.end()) {
		throw std::out_of_range("Cannot call Graph::GetConnected if src doesn't exist in the graph");
	}
	const auto& src = src_it->second;
	std::vector<N> result;
	for (auto& pair : src.edges) {
		result.push_back(*pair.first);
	}
	return result;
}

template <typename N, typename E>
E Graph<N, E>::GetWeight(const N& src, const N& dst) const {
	if (!IsNode(src) || !IsNode(dst)) {
		throw std::out_of_range(
		"Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
	}
	auto& src_node = nodes[src];
	if (src_node.edges.count(dst)) {
		return src_node.edges.find(dst).second;
	}
	throw std::out_of_range(
	"Cannot call Graph::GetWeights if src or dst node aren't connected");
}
template <typename N, typename E>
bool node_check(const graph::Graph<N, E>& lhs, const graph::Graph<N, E>& rhs) {
	for (const auto& node : lhs.GetNodes()) {
		if (!rhs.IsNode(node)) {
			return false;
		}
	}
	return true;
}

template <typename N, typename E>
bool edge_check(const Graph<N, E>& lhs, const Graph<N, E>& rhs) {
	for (const auto& src_it : lhs.nodes) {
		auto& src = src_it->second;
		for (const auto& dst_it : src) {
			if (dst_it->second != rhs.GetWeight(src, dst_it->first)) {
				return false;
			}
		}
	}
	return true;
}

template <typename N, typename E>
std::ostream& operator<<(std::ostream& os, const Graph<N, E>& graph) {
	for (auto pair : graph.nodes) {
		auto& node = pair.second;
		os << pair.first;
		os << " (\n";
		for (auto& edge_pair : node.edges) {
			auto edge = edge_pair.second;
			auto& dest = edge_pair.first;
			os << "  " << dest << " | " << edge << "\n";
		}
		os << ")\n";
	}
	return os;
}

template <typename N, typename E>
bool operator==(const Graph<N, E>& lhs, Graph<N, E>& rhs) {
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

}
