#include <iostream>
#include <vector>

int main(int argc, char const *argv[]) {
	std::vector<int> test{0,1,2,3,4,5,6};
	for(auto& i : test) {
		std::cout << i;
	}
	return 0;
}
