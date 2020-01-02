#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <math.h>
#include <algorithm>

#if __GNUC__ > 7
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#define old_fs
#endif


#include "import.h"
#include "graph.h"

#define data_location "data/recipes/"

struct craft_count {
	size_t count = 0;
	size_t needed = 0;
	bool ready = false;
	size_t distance = 0;
};

using recipe_graph_t = graph::Graph<std::string, int>;
using craft_store = std::unordered_map<std::string, craft_count>;
graph::Graph<std::string, int> build_graph(const std::vector<crafter::Ingredients>& requests, const crafter::recipe_store& recipes);
craft_store tally_count(const std::vector<crafter::Ingredients>& requests, const recipe_graph_t& recipe_graph, const crafter::recipe_store& recipes);
bool check_ingredient(const std::string& ingredient, craft_store& recipe_count, const recipe_graph_t& recipe_graph, const crafter::recipe_store& recipes);
std::vector<crafter::Ingredients> get_requests (const crafter::recipe_store& recipes, const std::string& input_file);
std::vector<crafter::Ingredients> get_requests_from_input (const crafter::recipe_store& recipes);
std::vector<std::vector<std::string>> get_order (const craft_store& recipe_count);
void output (const std::vector<std::vector<std::string>>& order, const craft_store&, const recipe_graph_t& recipe_graph);
void output_recipe(const std::string& name, const craft_store&, const recipe_graph_t& recipe_graph);
bool check_parent(const std::string& parent, craft_store& recipe_count, const recipe_graph_t& recipe_graph);
crafter::recipe_store read_templates(std::string template_location);
bool valid_extension(std::string);
std::string read_args(int argc, char const *argv[]);


template <typename N, typename E>
std::vector<N> heads(graph::Graph<N, E>);

template <typename N, typename E>
std::vector<N> tails(graph::Graph<N, E>);


int main(int argc, char const *argv[]) {
	auto input = read_args(argc, argv);

	auto recipes = read_templates(data_location);

	if (input == "") {
		std::cout << "Loaded " << recipes.size() << " recipes\n";
	}

	auto requests = get_requests(recipes, input);

    if (requests.size() == 0) {
        std::cout << "No input given\n";
        return 0;
    }

	auto recipe_graph = build_graph(requests, recipes);
	// std::cout << recipe_graph;
	auto recipe_counts = tally_count(requests, recipe_graph, recipes);
	auto simplified = get_order(recipe_counts);
	output(simplified, recipe_counts, recipe_graph);

	return 0;
}

graph::Graph<std::string, int> build_graph(const std::vector<crafter::Ingredients>& requests, const crafter::recipe_store& recipes) {
	std::deque<std::string> queue;
	std::unordered_set<std::string> seen;
	for (const auto& request : requests) {
		queue.push_back(request.name);
		seen.insert(request.name);
	}
	graph::Graph<std::string, int> graph_;
	while (!queue.empty()) {
		auto request = queue[0];
		queue.pop_front();
		graph_.InsertNode(request);
		auto recipe_it = recipes.find(request);
		if (recipe_it != recipes.end()) {
			auto& recipe = recipe_it->second[0];
			for (const auto& ingredient : recipe.ingredients) {
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
	for (const auto& node : g) {
		if (g.GetIncoming(node).size() == 0) {
			result.push_back(node);
		}
	}
	return result;
}

template <typename N, typename E>
std::vector<N> tails(graph::Graph<N, E> g) {
	std::vector<N> result;
	for (const auto& node : g) {
		if (g.GetConnected(node).size() == 0) {
			result.push_back(node);
		}
	}
	return result;
}


craft_store tally_count(const std::vector<crafter::Ingredients>& requests, const recipe_graph_t& recipe_graph, const crafter::recipe_store& recipes) {
	std::unordered_map<std::string, craft_count> recipe_count;
	std::deque<std::string> queue;
    const auto head_vec = heads(recipe_graph);
    std::unordered_set<std::string> head_set{head_vec.begin(), head_vec.end()};
	for (const auto& node : requests) {
		auto needed = static_cast<size_t>(node.count);
		auto& recipe = recipes.find(node.name)->second[0];
		auto count = (size_t) ceil(needed / (double) recipe.makes);
        if (head_set.count(node.name)) {
            recipe_count[node.name] = craft_count{count, needed, true, 0};    
        } else {
            recipe_count[node.name] = craft_count{count, needed, false, 0};    
        }
		queue.push_back(node.name);
	}
	while (!queue.empty()) {
		auto request = queue[0];
		queue.pop_front();
		for (const auto& ingredient : recipe_graph.GetConnected(request)) {
			auto ready = check_ingredient(ingredient, recipe_count, recipe_graph, recipes);
			if (ready) {
				queue.push_back(ingredient);
			}
		}
	}

	std::vector<size_t> distances;
	for (const auto& tail : tails(recipe_graph)) {
		distances.push_back(recipe_count[tail].distance);
	}

	size_t max_distance = *std::max_element(distances.cbegin(), distances.cend());

	for (auto& it : recipe_count) {
		it.second.ready = false;
	}

	for (const auto& tail : tails(recipe_graph)) {
		queue.push_back(tail);
		recipe_count[tail].distance = max_distance;
		recipe_count[tail].ready = true;
	}

	while (!queue.empty()) {
		auto request = queue[0];
		queue.pop_front();
		for (const auto& parent : recipe_graph.GetIncoming(request)) {
			auto ready = check_parent(parent, recipe_count, recipe_graph);
			if (ready) {
				queue.push_back(parent);
			}
		}
	}

	return recipe_count;
}

bool check_ingredient(const std::string& ingredient, craft_store& recipe_count, const recipe_graph_t& recipe_graph, const crafter::recipe_store& recipes) {
	craft_count count;
	if (recipe_count.count(ingredient) != 0) {
        count = recipe_count[ingredient];
        if (count.ready) {
            return true;
        }
	}
	decltype(count.distance) parent_distance = 0;
	for (const auto& parent : recipe_graph.GetIncoming(ingredient)) {
		if (!recipe_count[parent].ready) {
			return false;
		}
		count.needed += recipe_count[parent].count * recipe_graph.GetWeight(parent, ingredient);
		parent_distance = std::max(parent_distance, recipe_count[parent].distance);
	}
	count.distance = parent_distance + 1;
	auto recipe_it = recipes.find(ingredient);
	bool has_recipe = recipe_it != recipes.end();
	if (!has_recipe) {
		count.ready = true;
		count.count = count.needed;
	} else {
		auto& recipe = recipe_it->second[0];
		count.count = ceil(count.needed / (double) recipe.makes);
		count.ready = true;
	}
	recipe_count[ingredient] = count;
	return has_recipe;
}

bool check_parent(const std::string& parent, craft_store& recipe_count, const recipe_graph_t& recipe_graph) {
	size_t child_distance = -1;
	for (const auto& child : recipe_graph.GetConnected(parent)) {
		if (!recipe_count[child].ready) {
			return false;
		}
		child_distance = std::min(child_distance, recipe_count[child].distance);
	}
	recipe_count[parent].distance = child_distance - 1;
	recipe_count[parent].ready = true;
	return true;
}

std::vector<crafter::Ingredients> get_requests (const crafter::recipe_store& recipes, const std::string& input_file) {
	if (input_file == "") {
		return get_requests_from_input(recipes);
	} else {
		return crafter::get_requests_from_file(recipes, input_file);
	}
}


std::vector<crafter::Ingredients> get_requests_from_input (const crafter::recipe_store& recipes) {
	std::cout << "Input Recipe: ";
	std::string in;
	getline(std::cin, in);

	std::vector<crafter::Ingredients> requests;
	while (in != "") {
		auto it = recipes.find(in);
		if (it == recipes.end()) {
			std::cerr << "Recipe not found\n";
		} else {
			requests.push_back(crafter::Ingredients(in, 1));
		}
		std::cout << "Input Recipe: ";
		getline(std::cin, in);
	}
	std::cout << "\n";
	return requests;
}

std::vector<std::vector<std::string>> get_order (const craft_store& recipe_count) {
	std::vector<std::vector<std::string>> result;
	for (const auto& it : recipe_count) {
		auto& name = it.first;
		auto& craft = it.second;
		if (craft.distance >= result.size()) {
			result.resize(craft.distance + 1);
		}
		result[craft.distance].push_back(name);
	}
	for (auto& level : result) {
		std::sort(level.begin(), level.end());
	}
	return result;
}

void output (const std::vector<std::vector<std::string>>& order, const craft_store& craft, const recipe_graph_t& recipe_graph) {
	const std::string line = "---------------";
	size_t level_count = 0;
	for (auto level = order.crbegin(); level != order.crend(); level++) {
		level_count++;
		std::cout << line << " " << "Level " << level_count << " " << line << "\n\n";
		for (const auto& name : *level) {
			output_recipe(name, craft, recipe_graph);
		}
	}
}

void output_recipe(const std::string& name, const craft_store& craft, const recipe_graph_t& recipe_graph) {
	std::cout << name << " (" << craft.find(name)->second.count << ")\n";
	size_t count = craft.find(name)->second.count;
	for (const auto& ingredient : recipe_graph.GetConnected(name)) {
		std::cout << count * recipe_graph.GetWeight(name, ingredient) << "\t" << ingredient << "\n";
	}
	if (recipe_graph.GetConnected(name).size() != 0) {
		std::cout << "\n";
	}
}

bool valid_extension(std::string extension) {
	if (extension == ".yaml" || extension == ".yml") {
		return true;
	}
	return false;
}

#ifndef old_fs
crafter::recipe_store read_templates(std::string template_location) {
	crafter::recipe_store result;
	for (const auto& entry : fs::directory_iterator(template_location)) {
		if (entry.is_regular_file() && valid_extension(entry.path().extension())) {
			crafter::read_in(entry.path(), result);
		}
	}
	return result;
}
#else
crafter::recipe_store read_templates(std::string template_location) {
	crafter::recipe_store result;
	for (const auto& entry : fs::directory_iterator(template_location)) {
		if (fs::is_regular_file(entry) && valid_extension(entry.path().extension())) {
			crafter::read_in(entry.path(), result);
		}
	}
	return result;
}
#endif


std::string read_args (int argc, char const *argv[]) {
	if (argc == 1) {
		return "";
	} else if (argc == 2) {
		return argv[1];
	} else {
		std::cerr << "Got " << argc << " arguements, expected 0 or 1\n";
		throw std::invalid_argument(argv[2]);
	}
}
