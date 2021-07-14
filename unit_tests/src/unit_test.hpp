#pragma once
#include <source_location>
#include <functional>
#include <filesystem>
#include <iostream>
#include <string>
#include <format>
#include <memory>
#include <map>

#pragma region macro

// P3_UNIT_TEST(unique_name) { /* code */ }
#define P3_UNIT_TEST(name) namespace unit_test_cases\
{\
	struct name : unit_test::basic_test\
	{\
	public:\
		void run_test() const;\
	private:\
		static unit_test::test_factory_inserter<name> inserter;\
	};\
};\
unit_test::test_factory_inserter<unit_test_cases::name> unit_test_cases::name::inserter = unit_test::test_factory_inserter<unit_test_cases::name>(#name);\
void unit_test_cases::name::run_test() const

#pragma endregion

namespace unit_test
{
#pragma region constants

	namespace text
	{
		// \x1B[XXm <- XX is the 2 digit code
		constexpr auto black1   = "\x1B[30m";
		constexpr auto black2   = "\x1B[90m";
		constexpr auto red1     = "\x1B[31m";
		constexpr auto red2     = "\x1B[91m";
		constexpr auto green1   = "\x1B[32m";
		constexpr auto green2   = "\x1B[92m";		
		constexpr auto yellow1  = "\x1B[33m";
		constexpr auto yellow2  = "\x1B[93m";
		constexpr auto blue1    = "\x1B[34m";
		constexpr auto blue2    = "\x1B[94m";
		constexpr auto magenta1 = "\x1B[35m";
		constexpr auto magenta2 = "\x1B[95m";
		constexpr auto cyan1    = "\x1B[36m";
		constexpr auto cyan2    = "\x1B[96m";
		constexpr auto white1   = "\x1B[37m";
		constexpr auto white2   = "\x1B[97m";
	}

#pragma endregion
#pragma region types and concepts

	struct basic_test
	{
		virtual void run_test() const = 0;
	};
	using test_ptr = std::unique_ptr<basic_test>;

	struct basic_factory
	{
		virtual test_ptr create() const = 0;
	};
	using factory_ptr = std::unique_ptr<basic_factory>;
	using key_type = std::string;
	using result_type = bool;

	static inline std::map<key_type, factory_ptr> test_factories;

	template <typename test_type>
	concept valid_test = true; // = std::is_base_of<basic_test, test_type>::value;

#pragma endregion
#pragma region factory and inserter

	template <valid_test test_type>
	struct factory : basic_factory
	{
		const std::string name;

		factory(const std::string &name)
			: name{ name }
		{
		}

		test_ptr create() const
		{
			return std::make_unique<test_type>();
		}
	};

	template <valid_test data_type>
	struct test_factory_inserter
	{
		// this is the hack. by adding a static test_factory_inserter to a test case, we can use the following line to invoke:
		// unit_test::test_factory_inserter<name> test_case::inserter = unit_test::test_factory_inserter<test_case>("test_case");
		// which basically just invokes the registration of the test. wrap this into a macro and we're set.

		test_factory_inserter(const key_type &name)
		{
			std::clog << text::cyan2 << "registering test " << text::cyan1 << '"' << name << '"' << text::cyan2 << "..." << text::white1 << std::endl;
			test_factories[name] = std::make_unique<factory<data_type>>(name);
		}
	};

#pragma endregion
#pragma region actual test

	inline std::map<key_type, result_type> run()
	{
		std::map<key_type, result_type> result;
		for (const auto &[name, creator] : test_factories)
		{
			std::cout << std::endl << "running test \"" << name << "\"..." << std::endl;
			const auto &test = creator->create();

			bool success = true;
			try
			{
				test->run_test();
			}
			catch (const std::exception &e)
			{
				success = false;
				std::cout << text::yellow1 << " -> " << e.what() << text::white1 << std::endl;
			}

			const auto &color1 = success ? text::green1 : text::red1;
			const auto &color2 = success ? text::green2 : text::red2;

			std::cout << color2 << " -> test " << color1 << '"' << name << '"' << color2 << ' ' << (success ? "succeeded" : "failed") << text::white1 << std::endl;
			result[name] = success;
		}
		return result;
	}

#pragma endregion
#pragma region evaluation

	inline void evaluate(std::size_t successful, std::size_t total)
	{
		std::cout << "Final unit test summary:" << std::endl;

		const double percentage = 100.0 * successful / total;
		std::cout << std::format(" -> Successful tests: {2:.2f}% ({0}/{1})", successful, total, percentage) << std::endl;

		const std::string summary = successful == total ? "You may push your code! :)" : "Do not upload this code! :(";
		const auto color = successful == total ? text::green2 : text::red2;
		std::cout << color << " -> " << summary << text::white1 << std::endl;
	}

	inline void evaluate(const std::map<key_type, result_type> &results)
	{
		std::size_t successful = 0, total = 0;
		for (const auto &[name, result] : results)
		{
			if (result)
			{
				++successful;
			}
			++total;
		}
		evaluate(successful, total);
	}

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

	class assertion_fail : public std::exception
	{
		using std::exception::exception;
	};

	class assert_equals_fail : public assertion_fail
	{
		using assertion_fail::assertion_fail;
	};

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

	class assert_bool_fail : public assertion_fail
	{
		using assertion_fail::assertion_fail;
	};

#pragma endregion
}