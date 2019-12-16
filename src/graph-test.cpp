#include "graph.h"
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

template <typename T, typename V>
class _G3;

template <typename T, typename V>
bool operator==(const _G3<T, V> lhs, const _G3<T, V> rhs);

template <typename T, typename V>
class _G3 {
public:
	_G3(T c, V d) : a{c}, b{d} {}
	friend bool operator== <T, V>(const _G3<T, V> lhs, const _G3<T, V> rhs);
private:
	T a;
	V b;
};

template <typename T, typename V>
class G2 {
public:
	using G3 = _G3<T, V>;
};


template <typename T, typename V>
bool operator==(const _G3<T, V> lhs, const _G3<T, V> rhs) {
	return lhs.a == rhs.a && lhs.b == rhs.b;
}

int main() {
	graph::Graph<std::string, int> b, c;
	b.InsertNode("ho");
	b.InsertNode("humm");
	b.InsertEdge("ho", "humm", 5);
	// std::cout << b;

	G2<std::string, int>::G3 n("first", 1), m("second", 2), o("first", 1);

	if (n == m) {
		std::cout << "nope!\n";
	}
	if (n == o) {
		std::cout << "WTF!\n";
	}
	if (b == c) {
		std::cout << "hi!\n";
	}


	for (auto e = b.cbegin(); e != b.cend(); e++) {
		std::cout << *e << "\n";
	}
	for (const auto d : b) {
		std::cout << d << "\n";
	}
	test::Test<int, std::string> a;
	a.v = 9999;
	a.w = "testing";
	std::cout << a;
	return 0;
}
