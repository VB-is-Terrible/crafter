#include <iostream>
#include <fstream>
#include "yaml-cpp/yaml.h"


int main(int argc, char const *argv[]) {
	std::ifstream testfile("test.yaml");
	YAML::Node mine_template = YAML::LoadFile("test.yaml");
	YAML::Node panel = mine_template["Solar Panel II"];
	std::cout << "type: " << panel.Type() << "\n";
	std::cout << "type: " << panel["ingredients"].Type() << "\n";

	std::cout << "Makes: " << panel["makes"] << "\n";
	std::cout << "Makes: 1\n";
	return 0;
}
