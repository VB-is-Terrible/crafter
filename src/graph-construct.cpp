#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <deque>

#include "import.h"
#include "graph.h"

#define data_location "data/recipes/import.yaml"

struct craft_count {
	int count;
	bool ready;
};

graph::Graph<std::string, int> build_graph(std::vector<std::string> requests, const crafter::recipe_store& recipes);
std::unordered_map<std::string, craft_count> tally_count(graph::Graph<std::string, int> recipe_graph);


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

// std::unordered_map<std::string, craft_count> tally_count(graph::Graph<std::string, int> recipe_graph) {
//
// }
