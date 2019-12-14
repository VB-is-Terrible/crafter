#include "graph2.h"

namespace graph {
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
        for (const auto& src : lhs.GetNodes()) {
                for (const auto& dst : lhs.GetConnected(src)) {
                        if (lhs.GetWeight(src, dst) != rhs.GetWeight(src, dst)) {
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
                os << node.value;
                os << " (\n";
                for (auto edge_pair : node.edges) {
                        auto edge = edge_pair.second;
                        auto& dest = edge_pair.first;
                        os << "  " << dest << " | " << *edge << "\n";
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
