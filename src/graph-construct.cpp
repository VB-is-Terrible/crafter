#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <math.h>

#include "import.h"
#include "graph.h"

#define data_location "data/recipes/import.yaml"

struct craft_count {
	size_t count;
	size_t needed;
	bool ready;
	size_t distance;
};

graph::Graph<std::string, int> build_graph(std::vector<std::string> requests, const crafter::recipe_store& recipes);
std::unordered_map<std::string, craft_count> tally_count(graph::Graph<std::string, int> recipe_graph);
bool check_ingredient(const std::string& ingredient, std::unordered_map<std::string, craft_count>& recipe_count, const graph::Graph<std::string, int>& recipe_graph);

template <typename N, typename E>
std::vector<N> heads(graph::Graph<N, E>);

template <typename N, typename E>
std::vector<N> tails(graph::Graph<N, E>);


int main() {
	auto recipes = crafter::read_in(data_location);
	std::cout << "Loaded " << recipes.size() << " recipes\n";
	std::cout << "Input Recipe: ";
	std::string in;
	getline(std::cin, in);

	std::vector<std::string> requests;
	while (in != "") {
		auto it = recipes.find(in);
		if (it == recipes.end()) {
			std::cerr << "Recipe not found\n";
		} else {
			requests.push_back(in);
		}
		std::cout << "Input Recipe: ";
		getline(std::cin, in);
	}

	auto recipe_graph = build_graph(requests, recipes);
	std::cout << recipe_graph;
	// auto recipe_counts = tally_count(recipe_graph);

	return 0;
}

graph::Graph<std::string, int> build_graph(std::vector<std::string> requests, const crafter::recipe_store& recipes) {
	std::deque<std::string> queue;
	std::unordered_set<std::string> seen;
	for (auto request : requests) {
		queue.push_back(request);
		seen.insert(request);
	}
	graph::Graph<std::string, int> graph_;
	while (!queue.empty()) {
		auto request = queue[0];
		std::cout << "Unpacking " << request << "\n";
		queue.pop_front();
		graph_.InsertNode(request);
		auto recipe_it = recipes.find(request);
		if (recipe_it != recipes.end()) {
			auto& recipe = recipe_it->second[0];
			for (auto& ingredient : recipe.ingredients) {
				graph_.InsertNode(ingredient.name);
				graph_.InsertEdge(request, ingredient.name, ingredient.count);
				if (!seen.count(ingredient.name)) {
					seen.insert(ingredient.name);
					queue.push_back(ingredient.name);
				}
			}
		}
	}
	return graph_;
}

template <typename N, typename E>
std::vector<N> heads(graph::Graph<N, E> g) {
	std::vector<N> result;
	for (auto& node : g) {
		if (g.GetConnected(node).size() == 0) {
			result.push_back(node);
		}
	}
	return result;
}

template <typename N, typename E>
std::vector<N> tails(graph::Graph<N, E> g) {
	std::vector<N> result;
	for (auto& node : g) {
		if (g.GetIncoming(node).size() == 0) {
			result.push_back(node);
		}
	}
	return result;
}


std::unordered_map<std::string, craft_count> tally_count(const graph::Graph<std::string, int>& recipe_graph, const crafter::recipe_store& recipes) {
	std::unordered_map<std::string, craft_count> recipe_count;
	std::deque<std::string> queue;
	for (auto& node : heads(recipe_graph)) {
		recipe_count[node] = craft_count{1, true, 0};
		queue.push_back(node);
	}
	while (!queue.empty()) {
		auto request = queue[0];
		queue.pop_front();
		for (auto& ingredient : recipe_graph.GetConnected(request)) {
			auto ready = check_ingredient(ingredient, recipe_count, recipe_graph);
			if (ready) {
				queue.push_back(ingredient);
			}
		}
	}
}

bool check_ingredient(const std::string& ingredient, std::unordered_map<std::string, craft_count>& recipe_count, const graph::Graph<std::string, int>& recipe_graph, const crafter::recipe_store& recipes) {
	craft_count count;
	for (auto& parent : recipe_graph.getIncoming(ingredient)) {
		if (!recipe_count[parent].ready) {
			return false;
		}
		count.needed += recipe_count[parent].count;
	}
	count.count = ceil(count.needed / (double) )
	return true;
}
