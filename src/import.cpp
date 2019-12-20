#include "import.h"

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

#include "yaml-cpp/yaml.h"

namespace crafter {
	recipe_store read_in(std::string file_name) {
		std::ifstream fin(file_name);
		return read_in(fin);
	}

	recipe_store read_in(std::ifstream& file_name) {
		recipe_store recipes;
		auto recipes_yaml = YAML::Load(file_name);
		for (auto it : recipes_yaml) {
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
		return recipes;
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
		if (ingredients.IsDefined() && !ingredients.IsSequence()) {
			throw std::runtime_error("Failed to parse: " + name + "\n" + "Invalid 'ingredients' value");
		}
		if (ingredients.IsSequence()) {
			for (auto ingredient : ingredients) {
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
			for (auto ingredient_it : ingredients) {
				std::string ingredient;
				int count;
				try {
					name = ingredient_it.first.as<std::string>();
					count = ingredient_it.second.as<int>();
				} catch (...) {
					throw std::runtime_error("Failed to read in a ingredient list for " + name + "\nThe yaml format seems to be stuffed");
				}
				this->ingredients.push_back(Ingredients{count, ingredient})
			}
		}

	}

	Ingredients::Ingredients(YAML::Node ingredient) {
		this->count = ingredient["count"].as<int>();
		this->name = ingredient["name"].as<std::string>();
	}
}
