#include "import.h"

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <filesystem>

#include "yaml-cpp/yaml.h"

namespace crafter {
	recipe_store read_in(std::string file_name) {
		std::ifstream fin(file_name);
		return read_in(fin);
	}

	void read_in(std::string file_name, recipe_store& store) {
		std::ifstream fin(file_name);
		return read_in(fin, store);
	}

	recipe_store read_in(std::ifstream& file) {
		recipe_store recipes;
		read_in(file, recipes);
		return recipes;
	}

	void read_in(std::ifstream& file, recipe_store& recipes) {
		auto recipes_yaml = YAML::Load(file);
		for (const auto it : recipes_yaml) {
			std::string name;
			try {
				name = it.first.as<std::string>();
			} catch (...) {
				throw std::runtime_error("Failed to read in a recipe name\nThe yaml format seems to be stuffed");
			}
			auto recipe = it.second;
			if (recipes.count(name) == 0) {
				recipes[name] = std::vector<Recipe>();
			}
			recipes[name].push_back(Recipe(name, recipe));
		}
	}

	Recipe::Recipe(std::string name, YAML::Node recipe) : name{name} {
		auto makes = recipe["makes"];
		if (makes.IsDefined() && !makes.IsScalar()) {
			throw std::runtime_error("Failed to parse: " + name + "\n" + "Invalid 'makes' value");
		}
		if (makes.IsScalar()) {
			this->makes = makes.as<int>();
		}
		auto ingredients = recipe["ingredients"];
		if (ingredients.IsDefined() && !(ingredients.IsSequence() || ingredients.IsMap())) {
			throw std::runtime_error("Failed to parse: " + name + "\n" + "Invalid 'ingredients' value");
		}
		if (ingredients.IsSequence()) {
			for (const auto ingredient : ingredients) {
				if (!ingredient.IsMap()) {
					throw std::runtime_error("Failed to parse: " + name + "\n" + "Invalid 'ingredient' value");
				}
				try {
					this->ingredients.push_back(Ingredients(ingredient));
				} catch (...) {
					throw std::runtime_error("Failed to parse: " + name + "\n" + "Invalid 'ingredient' value");
				}
			}
		} else if (ingredients.IsMap()) {
			for (const auto ingredient_it : ingredients) {
				std::string ingredient;
				int count;
				try {
					ingredient = ingredient_it.first.as<std::string>();
					count = ingredient_it.second.as<int>();
				} catch (...) {
					throw std::runtime_error("Failed to read in a ingredient list for " + name + "\nThe yaml format seems to be stuffed");
				}
				this->ingredients.push_back(Ingredients(ingredient, count));
			}
		}

	}

	Ingredients::Ingredients(YAML::Node ingredient) {
		this->count = ingredient["count"].as<int>();
		this->name = ingredient["name"].as<std::string>();
	}

	std::vector<Ingredients> get_requests_from_file(const crafter::recipe_store& recipes, const std::string& input_file) {
		auto requests_yaml = YAML::LoadFile(input_file);
		std::vector<Ingredients> requests;
		if (requests_yaml.IsSequence()) {
			for (const auto name_node : requests_yaml) {
				std::string name;
				try {
					name = name_node.as<std::string>();
				} catch (...) {
					std::cerr << "Failed to read request from file\n";
					continue;
				}
				if (recipes.count(name)) {
					requests.push_back(Ingredients(name, 1));
				} else {
					std::cerr << "Could not find a recipe for " << name << "\n";
				}

			}
		} else if (requests_yaml.IsMap()) {
			for (const auto request_it : requests_yaml) {
				std::string name;
				int count;
				try {
					name = request_it.first.as<std::string>();
					count = request_it.second.as<int>();
				} catch (...) {
					std::cerr << "Failed to read request from file\n";
					continue;
				}

				if (recipes.count(name)) {
					requests.push_back(Ingredients(name, count));
				} else {
					std::cerr << "Could not find a recipe for " << name << "\n";
				}
			}
		} else if (requests_yaml.IsScalar()){
			std::string name;
			try {
				name = requests_yaml.as<std::string>();
			} catch (...) {
				std::cerr << "Failed to read request from file\n";
			}
			if (name != "") {
				requests.push_back(Ingredients(name, 1));
			}
		}
		return requests;
	}

}
