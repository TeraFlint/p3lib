#pragma once
#include "../unit_test.hpp"
#include <vector>

import p3.meta;

P3_UNIT_TEST(meta_list_for_each_empty)
{
	using types = p3::meta::types<>;
	types::for_each_type([&]<typename t>() { unit_test::assert_equals(0, 1, "empty typelist should never iterate"); });
}

P3_UNIT_TEST(meta_list_for_each)
{
	using types = p3::meta::types<int, void, char, p3::meta::types<double>>;
	const std::vector<std::string> expected = { "int", "void", "char", "struct p3::meta::types_1<double>"};

	std::vector<std::string> actual;
	types::for_each_type([&]<typename t>()
	{
		const std::string name = typeid(t).name();
		std::cout << " - " << name << std::endl;
		actual.push_back(name);
	});

	unit_test::assert_equals(expected.size(), actual.size(), "size missmatch");

	auto expected_iter = expected.begin();
	auto actual_iter = actual.begin();
	for (; expected_iter != expected.end(); ++expected_iter, ++actual_iter)
	{
		unit_test::assert_equals(*expected_iter, *actual_iter, "item mismatch");
	}
}

P3_UNIT_TEST(meta_list_for_each_interoperability)
{
	using types = p3::meta::types<bool, int, float, double, short>;

	auto function = [name = 'a'](auto&& obj = {}) mutable
	{
		std::cout << " - const " << typeid(decltype(obj)).name() << "\t" << name++ << " = " << obj << ";" << std::endl;
	};

	std::cout << "for_each_value:" << std::endl;
	types::for_each_value(function, -1337.42);
	std::cout << "for_each_type:" << std::endl;
	types::for_each_type(function);

	// test passes if it compiles.
}