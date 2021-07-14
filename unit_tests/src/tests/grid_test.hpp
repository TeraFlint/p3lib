#pragma once
#include "../unit_test.hpp"

import p3.grid;

#pragma region nested_list

/*P3_UNIT_TEST(grid_nested_lists)
{
	// apparently the IDE is not very happy with the type shenannigans that I did in the module.
	// it's perfectly fine for the compiler, though.

	p3::nested_list_t<int, 0> list_0 = 1;
	p3::nested_list_t<int, 1> list_1 = { 1, 2, 3, 4 };
	p3::nested_list_t<int, 2> list_2 = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
	p3::nested_list_t<int, 3> list_3 = { { { 1, 2 }, { 3, 4 } }, { { 5, 6 }, { 7, 8 } } };
}*/

#pragma endregion
#pragma region grid_size

P3_UNIT_TEST(grid_size_elements)
{
	constexpr p3::grid_size<4> size{ 2, 3, 5, 7 };
	unit_test::assert_equals<size_t>(210U, size.elements(), "grid_size::elements()");
}

P3_UNIT_TEST(grid_size_index)
{
	constexpr p3::grid_size<3> boundary{ 5, 4, 3 };
	static_assert( 0 == boundary.index_of({ 0, 0, 0 }), "grid_size::index_of({ 0, 0, 0 })");
	static_assert( 1 == boundary.index_of({ 0, 0, 1 }), "grid_size::index_of({ 0, 0, 1 })");
	static_assert( 3 == boundary.index_of({ 0, 1, 0 }), "grid_size::index_of({ 0, 1, 0 })");
	static_assert(12 == boundary.index_of({ 1, 0, 0 }), "grid_size::index_of({ 1, 0, 0 })");
	static_assert(16 == boundary.index_of({ 1, 1, 1 }), "grid_size::index_of({ 1, 1, 1 })");
	static_assert(59 == boundary.index_of({ 4, 3, 2 }), "grid_size::index_of({ 4, 3, 2 })");
	static_assert( 7 == boundary.index_of({ 0, 2, 1 }), "grid_size::index_of({ 0, 2, 1 })");

	for (size_t i = 0; i < boundary.elements(); ++i)
	{
		const auto position = p3::grid_size<3>::from_index(i, boundary);
		const auto index = position.index_in(boundary);
		unit_test::assert_equals<size_t>(i, index, "grid_size::index_in()");
	}
}

P3_UNIT_TEST(grid_size_fitting)
{
	constexpr p3::grid_size<2> size{ 1, 5 };
	constexpr auto elements = size.elements();

	static_assert(1 == size.fit_to_data(4)[0]);
	static_assert(1 == size.fit_to_data(5)[0]);
	static_assert(2 == size.fit_to_data(6)[0]);
}

#pragma endregion
#pragma region grid_pos

P3_UNIT_TEST(grid_pos_constructor)
{
	constexpr p3::grid_size<3> empty_size{};
	constexpr p3::grid_size<3> size = { 3, 5, 7 };

	constexpr p3::grid_pos<3> default_pos{};
	constexpr p3::grid_pos<3> size_pos{ size };

	static_assert(default_pos.valid(), "grid_pos::valid()");
	static_assert(default_pos.pos() == empty_size, "grid_pos::pos()");
	static_assert(default_pos.dim() == empty_size, "grid_pos::dim()");

	static_assert(size_pos.valid(), "grid_pos::valid()");
	static_assert(size_pos.pos() == empty_size, "grid_pos::pos()");
	static_assert(size_pos.dim() ==       size, "grid_pos::dim()");
}

P3_UNIT_TEST(grid_pos_comparison)
{
	constexpr p3::grid_size<4> size = { 3, 4, 5, 3 };
	constexpr p3::grid_size<4> pos = { 2, 2, 1, 2 };

	constexpr p3::grid_pos<4> direct{ size, pos };
	static_assert(direct == direct, "grid_pos: object did not equal itself");

	p3::grid_pos<4> jump{ size };
	if (jump != jump)
	{
		unit_test::assert_equals(0, 1, "grid_pos: object did not equal itself");
	}
	if (direct == jump)
	{
		unit_test::assert_equals(0, 1, "grid_pos: two differently valued object achieved equality");
	}

	jump.jump(pos);
	if (direct != jump)
	{
		unit_test::assert_equals(0, 1, "grid_pos: two equaly valued object did not achieve equality");
	}
}

P3_UNIT_TEST(grid_pos_jump)
{
	p3::grid_pos<3> pos{ {2, 4, 8} };

	const auto assert_jump = [&](bool expected, const p3::grid_size<3> &to_pos)
	{
		const auto actual = pos.jump(to_pos);
		unit_test::assert_equals<bool>(expected, actual, std::format("grid_pos::jump({{{0}, {1}, {2}}})", to_pos[0], to_pos[1], to_pos[2]));
	};

	assert_jump(true, { 0, 0, 0 });
	assert_jump(true, { 1, 1, 1 });
	assert_jump(true, { 1, 2, 3 });
	assert_jump(true, { 1, 3, 2 });
	assert_jump(true, { 1, 3, 7 });

	assert_jump(false, { 2, 0, 0 });
	assert_jump(false, { 0, 4, 0 });
	assert_jump(false, { 0, 0, 8 });
	assert_jump(false, { 2, 4, 8 });
}

P3_UNIT_TEST(grid_pos_next_prev)
{
	p3::grid_pos<2> pos{ {7, 3} };
	pos.jump({ 3, 2 });

	const auto copy1 = pos;
	pos.next();
	const auto copy2 = pos;
	pos.prev();
	const auto copy3 = pos;

	if (copy1 != copy3)
	{
		unit_test::assert_equals(0, 1, "grid_pos: next(); prev(); copies are not equal");
	}

	unit_test::assert_equals<size_t>(copy1.index() + 1, copy2.index(), "grid_pos::index() is not higher than the old value");
	unit_test::assert_equals<size_t>(copy1.index(), copy3.index(), "grid_pos::index() did not return to the old value");
}

P3_UNIT_TEST(grid_pos_index)
{
	constexpr p3::grid_size<4> size{ 7, 3, 5, 2 };
	constexpr auto elements = size.elements();

	size_t index = 0;
	for (p3::grid_pos<4> pos{ size }; pos.valid(); ++pos, ++index)
	{
		unit_test::assert_equals<size_t>(index, pos.index(), "grid_pos::index()");
	}
	unit_test::assert_equals<size_t>(elements, index, "grid_pos final count of iterations");
}

P3_UNIT_TEST(grid_pos_overflow)
{
	p3::grid_pos<3> pos{ {2, 2, 2} };
	unit_test::assert_equals<bool>(true, pos.valid(), "grid_pos::valid()");
	pos.last();
	unit_test::assert_equals<bool>(true, pos.valid(), "grid_pos::valid()");
	pos.next();
	unit_test::assert_equals<bool>(false, pos.valid(), "grid_pos::valid()");
	pos.first();
	unit_test::assert_equals<bool>(true, pos.valid(), "grid_pos::valid()");
}

#pragma endregion
#pragma region grid

P3_UNIT_TEST(grid_constructor_default)
{
	const p3::grid<int, 3> g = {};
}

P3_UNIT_TEST(grid_constructor_size)
{
	constexpr p3::grid_size<3> size{ 2, 3, 2 };
	const p3::grid<int, 3> grid(size);

	unit_test::assert_equals<size_t>(3, grid.rank(), "grid::rank()");

	for (size_t i = 0; i < size.size(); ++i)
	{
		unit_test::assert_equals<size_t>(size[i], grid.dim()[i],  std::format("grid::dim()[{0}]", i));
		unit_test::assert_equals<size_t>(size[i], grid.dim_at(i), std::format("grid::dim_at({0})", i));
	}
}

P3_UNIT_TEST(grid_constructor_size_complete_list)
{
	const p3::grid<unsigned, 2> grid{ {3, 3}, {1, 2, 3, 4, 5, 6, 7, 8, 9} };
	
	size_t index = 1;
	for (size_t y = 0; y < grid.dim_at(1); ++y)
	{
		for (size_t x = 0; x < grid.dim_at(0); ++x)
		{
			unit_test::assert_equals(index, grid.at({y, x}), std::format("grid::at({{{0}, {1}}})", y, x));
			++index;
		}
	}
}

P3_UNIT_TEST(grid_constructor_size_incomplete_list)
{
	const p3::grid<unsigned, 3> grid{ { 2, 2, 2 }, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 } };
	unit_test::assert_equals<size_t>(4, grid.dim_at(0), "grid::dim_at(0)");

	const size_t incrementing_until = 13;
	for (size_t i = 0; i < incrementing_until; ++i)
	{
		unit_test::assert_equals<size_t>(i+1, grid[i], "value initialized part");
	}
	for (size_t i = incrementing_until; i < grid.size(); ++i)
	{
		unit_test::assert_equals<size_t>(0, grid[i], "zero-initialized part");
	}
}

P3_UNIT_TEST(grid_constructor_generator)
{
	const auto operation = [](auto a, auto b) { return 2 * a + b; };

	constexpr p3::grid_size<2> size{ 10, 10 };
	const p3::grid<int, 2> grid{ size, [&](const auto &pos) { return operation(pos.pos_at(0), pos.pos_at(1)); } };
	
	for (size_t i = 0; i < grid.rank(); ++i)
	{
		unit_test::assert_equals<size_t>(size[i], grid.dim_at(i));
	}

	for (size_t y = 0; y < grid.dim_at(0); ++y)
	{
		for (size_t x = 0; x < grid.dim_at(1); ++x)
		{
			const auto expected = operation(y, x);
			const auto actual = grid.at({y, x});
			unit_test::assert_equals<int>(expected, actual, std::format("grid::at({{{0}, {1}}})", x, y));
		}
	}
}

P3_UNIT_TEST(grid_constructor_converter)
{
	constexpr p3::grid_size<3> size{ 3, 4, 5 };
	constexpr auto elements = size.elements();
	const auto generator = [](const p3::grid_pos<3> &pos) { return pos.pos_at(0) * pos.pos_at(1) + pos.pos_at(2); };
	const auto converter = [](const p3::grid_pos<3> &pos, const auto &old) { return old - 10; };

	const p3::grid<int, 3> generated{ size, generator };
	const p3::grid<float, 3> copy = generated;
	const p3::grid<int, 3> converted{ generated, converter };

	for (size_t i = 0; i < converted.rank(); ++i)
	{
		unit_test::assert_equals<size_t>(size[i], converted.dim_at(i));
	}

	// still no iterators implemented, sorry...
	for (size_t i = 0; i < elements; ++i)
	{
		const auto expected = generated[i] - 10;
		const auto actual = converted[i];
		unit_test::assert_equals<int>(expected, actual, std::format("grid::operator[]({0})", i));
	}
}

P3_UNIT_TEST(grid_value_assignment)
{
	constexpr p3::grid_size<2> position = { 1, 2 };
	constexpr char val1 = 'a';
	constexpr char val2 = 'b';

	p3::grid<char, 2> grid{ { 4, 3 } };

	const auto position_index = [&]() { return grid.dim().index_of(position); };
	const auto get_expected = [&](size_t index, char val) { return index == position_index() ? val : '\0'; };

	// sorry, iterators aren't implemented, yet.
	grid.at(position) = val1;
	for (size_t i = 0; i < grid.size(); ++i)
	{
		const auto actual = grid[i];
		const auto expected = get_expected(i, val1);
		unit_test::assert_equals<char>(expected, actual, std::format("grid::at() = {0};", val1));
	}

	grid[position_index()] = val2;
	for (size_t i = 0; i < grid.size(); ++i)
	{
		const auto actual = grid[i];
		const auto expected = get_expected(i, val2);
		unit_test::assert_equals<char>(expected, actual, std::format("grid::operator[]() = {0};", val2));
	}
}

P3_UNIT_TEST(grid_iterators)
{
	constexpr p3::grid_size<2> size{ 16, 16 };
	const p3::grid<int, 2> grid{ size, [](const auto &pos) { return pos.pos_at(0) ^ pos.pos_at(1); } };

	size_t counter = 0;
	for (const auto &elem : grid)
	{
		++counter;
	}
	unit_test::assert_equals<size_t>(size.elements(), counter, "grid: iteration count");

	auto copy = grid;
	auto grid_iter = grid.begin();
	auto copy_iter = copy.begin();
	
	for (; grid_iter != grid.end() && copy_iter != copy.end(); ++grid_iter, ++copy_iter)
	{
		unit_test::assert_equals<int>(*grid_iter, *copy_iter, "grid: dual iteration element comparison");
	}
	if (grid_iter != grid.end() || copy_iter != copy.end())
	{
		unit_test::assert_equals(1, 0, "grid: iterator count didn't match");
	}

	for (auto &elem : copy)
	{
		elem = 0;
	}
	for (const auto &elem : copy)
	{
		unit_test::assert_equals<int>(0, elem, "grid: iterator writing didn't work");
	}
}

P3_UNIT_TEST(grid_comparison)
{
	p3::grid<int, 3> default1, default2;
	if (default1 != default2)
	{
		unit_test::assert_equals(0, 1, "grid: default constructed elements did not achieve equality");
	}

	// ...

}

#pragma endregion