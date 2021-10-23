#include "tests/_all.hpp"

int main(int argc, char **argv)
{
	auto results = p3::unit_test::run();
	std::cout << std::endl;
	p3::unit_test::evaluate(results);
}