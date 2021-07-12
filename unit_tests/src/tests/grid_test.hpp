#pragma once
#include "../unit_tests.hpp"

import p3.grid;

/*P3_UNIT_TEST(grid_nested_lists)
{
	// apparently the IDE is not very happy with the type shenannigans that I did in the module.
	// it's perfectly fine for the compiler, though.

	p3::nested_list_t<int, 0> list_0 = 1;
	p3::nested_list_t<int, 1> list_1 = { 1, 2, 3, 4 };
	p3::nested_list_t<int, 2> list_2 = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
	p3::nested_list_t<int, 3> list_3 = { { { 1, 2 }, { 3, 4 } }, { { 5, 6 }, { 7, 8 } } };
}*/

P3_UNIT_TEST(grid_size_elements)
{
	constexpr p3::grid_size<4> size = { 2, 3, 5, 7 };
	unit_test::assert_equals<size_t>(210U, size.elements(), "grid_size::elements()");
}

P3_UNIT_TEST(grid_size_index)
{
	constexpr p3::grid_size<3> boundary = { 5, 4, 3 };
	static_assert( 0 == boundary.index_of({ 0, 0, 0 }), "boundary::index_of({ 0, 0, 0 })");
	static_assert( 1 == boundary.index_of({ 0, 0, 1 }), "boundary::index_of({ 0, 0, 1 })");
	static_assert( 3 == boundary.index_of({ 0, 1, 0 }), "boundary::index_of({ 0, 1, 0 })");
	static_assert(12 == boundary.index_of({ 1, 0, 0 }), "boundary::index_of({ 1, 0, 0 })");
	static_assert(59 == boundary.index_of({ 4, 3, 2 }), "boundary::index_of({ 4, 3, 2 })");
	static_assert( 7 == boundary.index_of({ 0, 2, 1 }), "boundary::index_of({ 0, 2, 1 })");
}

P3_UNIT_TEST(grid_constructor_default)
{
	const p3::grid<int, 3> g = {};
}

P3_UNIT_TEST(grid_constructor_size)
{
	constexpr p3::grid_size<3> size = { 2, 3, 2 };
	const p3::grid<int, 3> grid(size);

	unit_test::assert_equals<size_t>(3, grid.rank(), "grid::rank()");

	for (size_t i = 0; i < size.size(); ++i)
	{
		unit_test::assert_equals(size[i], grid.dimensions()[i], std::format("grid::dimensions()[{0}]", i));
		unit_test::assert_equals(size[i], grid.dimension(i),    std::format("grid::dimensions({0})", i));
	}
}

P3_UNIT_TEST(grid_constructor_size_matching_list)
{
	const p3::grid<unsigned, 2> grid({ 3, 3 }, { 1, 2, 3, 4, 5, 6, 7, 8, 9 });
	
	/*size_t index = 0;
	for (size_t y = 0; y < grid.dimension(1); ++y)
	{
		for (size_t x = 0; x < grid.dimension(0); ++x)
		{
			unit_test::assert_equals(index, grid.at({y, x}), std::format("grid::at({{0}, {1}})", y, x));
			++index;
		}
	}*/
}