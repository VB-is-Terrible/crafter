#include <iostream>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include <string>

int main() {
	std::ifstream testfile("test.yaml");
	YAML::Node mine_template = YAML::LoadFile("test.yaml");
	std::cout << "whole: " << mine_template << "\n";
	std::cout << "Partial: \n";
	for (auto r : mine_template) {
		std::cout << r.first.as<std::string>() << "\n";
	}
	YAML::Node panel = mine_template["Solar Panel II"];
	std::cout << "type: " << panel.Type() << "\n";
	std::cout << "type: " << panel["ingredients"].Type() << "\n";

	std::cout << "Makes: " << panel["makes"] << "\n";
	std::cout << "Makes: 1\n";
	return 0;
}
