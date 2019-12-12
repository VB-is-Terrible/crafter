#pragma once

#include <map>
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
		int count;
		std::string name;
	};
	struct Recipe {
		Recipe (std::string, YAML::Node);
		std::string name;
		int makes = 1;
		std::vector<Ingredients> ingredients;
	};

	using recipe_store = std::map<std::string, std::vector<Recipe>>;
	recipe_store read_in(std::ifstream& file);
	recipe_store read_in(std::string file_name);

}
