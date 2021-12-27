#pragma once
#include <source_location>
#include <functional>
#include <filesystem>
#include <iostream>
#include <string>
#include <format>
#include <memory>
#include <map>

import p3.unit_test;

// P3_UNIT_TEST(unique_name) { /* code */ }
#define P3_UNIT_TEST(name) struct name : p3::unit_test::test_case<name> { void run_test() const; }; void name::run_test() const


namespace unit_test
{
#pragma region exceptions

	class assertion_fail : public std::exception
	{
		using std::exception::exception;
	};

	class assert_equals_fail : public assertion_fail
	{
		using assertion_fail::assertion_fail;
	};

	class assert_bool_fail : public assertion_fail
	{
		using assertion_fail::assertion_fail;
	};

#pragma endregion
#pragma region assertions

	template <bool useless = true>
	std::string source_location_string(const std::source_location location = std::source_location::current())
	{
		std::filesystem::path file = location.file_name();
		return std::format("{0}() in {3}({1},{2})", location.function_name(), location.line(), location.column(), file.filename().string());
	}

	template <typename data_type>
	std::string assert_equals_message(const data_type &expected, const data_type &actual, const std::string &header, const std::string &body, const std::source_location location)
	{
		const std::string modified_body = body.empty() ? std::string{} : std::format(" ({0})", body);
		return std::format("{0}: expected {1} but got {2}{3}, at {4}", header, expected, actual, modified_body, source_location_string(location));
	}



	template <typename data_type>
	void assert_equals(const data_type &expected, const data_type &actual, const std::string &body = {}, const std::source_location location = std::source_location::current())
	{
		if (expected != actual)
		{
			const std::string msg = assert_equals_message(expected, actual, "assert_equals", body, location);
			throw assert_equals_fail(msg.c_str());
		}
	}

	template <typename iter_type1, typename iter_type2>
	void assert_equals(const iter_type1 &expected_begin, const iter_type1 &expected_end, const iter_type2 &actual_begin, const iter_type2 &actual_end, const std::string body = {}, const std::source_location location = std::source_location::current())
	{
		std::size_t index = 0;
		auto expected = expected_begin;
		auto actual = actual_begin;
		for (; expected != expected_end && actual != actual_end; ++expected, ++actual)
		{
			if (*expected != *actual)
			{
				const std::string msg = assert_equals_message(expected, actual, "assert_equals (element #" + std::to_string(index) + ")", body, location);
				throw assert_equals_fail(msg.c_str());
			}
		}
	}



#pragma endregion
}