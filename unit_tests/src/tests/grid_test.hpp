#pragma once
#include "../unit_tests.hpp"

import p3.grid;

P3_UNIT_TEST(grid_nested_lists)
{
	// apparently the IDE is not very happy with the type shenannigans that I did in the module.
	// it's perfectly fine for the compiler, though.

	constexpr p3::nested_list_t<int, 0> list_0 = 1;
	constexpr p3::nested_list_t<int, 1> list_1 = { 1, 2, 3, 4 };
	constexpr p3::nested_list_t<int, 2> list_2 = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
	constexpr p3::nested_list_t<int, 3> list_3 = { { { 1, 2 }, { 3, 4 } }, { { 5, 6 }, { 7, 8 } } };
}

P3_UNIT_TEST(grid_size_elements)
{
	constexpr p3::grid_size<3> size = { 2, 3, 5 };
	unit_test::assert_equals<size_t>(30U, size.elements(), "grid_size::elements()");
}

P3_UNIT_TEST(grid_default_constructor)
{
	p3::grid<int, 3> g{};
}
