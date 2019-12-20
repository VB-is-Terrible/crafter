#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include "yaml-cpp/yaml.h"

namespace crafter {
	struct StackType {
		enum value {Block, Liquid, NonStackable};
	};
	struct Ingredients {
		Ingredients (YAML::Node);
		Ingredients(std::string name_, int count_) : name{name_}, count{count_} {};
		std::string name;
		int count;
	};
	struct Recipe {
		Recipe (std::string, YAML::Node);
		std::string name;
		int makes = 1;
		std::vector<Ingredients> ingredients;
	};

	using recipe_store = std::unordered_map<std::string, std::vector<Recipe>>;
	recipe_store read_in(std::ifstream& file);
	recipe_store read_in(std::string file_name);
	void read_in(std::ifstream& file, recipe_store& store);
	void read_in(std::string file_name, recipe_store& store);

}
