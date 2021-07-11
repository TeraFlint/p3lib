#include "unit_tests.hpp"

import p3.grid;

P3_UNIT_TEST(succeed)
{
	unit_test::assert_equals(1, 1);
}

P3_UNIT_TEST(fail)
{
	unit_test::assert_equals(1, 2);
}

int main(int argc, char **argv)
{
	unit_test::run();
	p3::grid<int, 2> g;
}