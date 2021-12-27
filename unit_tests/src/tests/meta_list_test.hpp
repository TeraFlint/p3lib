#pragma once
#include "../unit_test.hpp"
#include <vector>

import p3.meta;

P3_UNIT_TEST(meta_list_for_each_empty)
{
	using types = p3::meta::type_list<>;
	types::for_each_type([&]<typename t>() { unit_test::assert_equals(0, 1, "empty typelist should never iterate"); });
}

P3_UNIT_TEST(meta_list_for_each)
{
	using types = p3::meta::type_list<int, void, char, p3::meta::type_list<double>>;
	const std::vector<std::string> expected = { "int", "void", "char", "struct p3::meta::type_list<double>"};

	std::vector<std::string> actual;
	types::for_each_type([&]<typename t>()
	{
		const std::string name = typeid(t).name();
		std::cout << "  " << name << std::endl;
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
	using types = p3::meta::type_list<bool, int, float, double>;
	using values = p3::meta::value_list<true, 2, 13.37f, 3.14159>;

	auto function = [name = 'a'](auto&& obj = {}) mutable
	{
		std::cout << "  const " << typeid(decltype(obj)).name() << "\t";
		std::cout << name++ << " = " << obj << ";" << std::endl;
	};

	std::cout << "type_list<>::for_each_value(function, 13.37)" << std::endl;
	types::for_each_value(function, 13.37);
	std::cout << "type_list<>::for_each_value(function)" << std::endl;
	types::for_each_value(function);
	std::cout << "type_list<>::for_each_type(function)" << std::endl;
	types::for_each_type(function);
	std::cout << "value_list<>::for_each_value(function)" << std::endl;
	values::for_each_value(function);

	// test passes if it compiles.
}