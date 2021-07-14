#include "tests/_all.hpp"

int main(int argc, char **argv)
{
	auto results = unit_test::run();
	std::cout << std::endl;
	unit_test::evaluate(results);


	const p3::grid<int, 2> board{ {16, 16}, [](const auto &pos) { return pos.pos_at(0) ^ pos.pos_at(1); } };
	auto iter = board.begin();
	for (size_t y = 0; y < board.dim_at(0); ++y)
	{
		for (size_t x = 0; x < board.dim_at(1); ++x, ++iter)
		{
			std::cout << std::format("{0:4d}", *iter);
		}
		std::cout << std::endl;
	}
}