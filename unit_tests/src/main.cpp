#include "tests/_all.hpp"

int main(int argc, char **argv)
{
	auto results = unit_test::run();
	std::cout << std::endl;
	unit_test::evaluate(results);
}