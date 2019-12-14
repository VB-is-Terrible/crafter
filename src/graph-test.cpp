#include "graph2.h"
#include <iostream>
#include <string>

namespace test {
// Forward declare both templates:
template <typename T, typename V> class Test;
template <typename T, typename V> std::ostream& operator<<( std::ostream&, const Test<T, V>& );
}

namespace test {
// Declare the actual templates:
template <typename T, typename V>
class Test {
	friend std::ostream& operator<< <T, V>( std::ostream&, const Test<T, V>& );
public:
	T v;
	V w;
};
// Implement the operator
template <typename T, typename V>
std::ostream& operator<<( std::ostream& o, const Test<T, V>& t ) {
	// Can only access Test<T> for the same T as is instantiating, that is:
	// if T is int, this template cannot access Test<double>, Test<char> ...
	o << t.v << " " << t.w << "\n";
	return o;
}

}

int main() {
	// graph::Graph<std::string, int> b;
	// std::cout << b;

	test::Test<int, std::string> a;
	a.v = 9999;
	a.w = "testing";
	std::cout << a;
	return 0;
}
