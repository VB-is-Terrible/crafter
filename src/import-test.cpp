#include "import.h"
#include <iostream>

int main() {
	auto recipes = crafter::read_in("data/recipes/import.yaml");
	std::string in;
	std::cout << "Loaded " << recipes.size() << " recipes\n";
	std::cout << "Input Recipe: ";
	getline(std::cin, in);
	while (in != "") {
		auto it = recipes.find(in);
		if (it == recipes.end()) {
			std::cout << "Recipe not found\n";
		} else {
			std::cout << "Makes: " << it->second[0].makes << "\n";
			std::cout << "Ingredients: \n";
			for (auto ingredient : it->second[0].ingredients) {
				std::cout << " - name:  " << ingredient.name << "\n";
				std::cout << "   count: " << ingredient.count << "\n";
			}
		}
		std::cout << "Input Recipe: ";
		getline(std::cin, in);
	}
	return 0;
}
