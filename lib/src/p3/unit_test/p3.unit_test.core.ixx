#include <iostream>
#include <memory>
#include <string>
#include <format>
#include <chrono>
#include <map>
export module p3.unit_test.core;
/*
	Unit test core module, part of github/TeraFlint/pitrilib.
	Daniel Wiegert (Pitri), 2021.
*/

export import p3.console_format;
// import <iostream>;
// import <memory>;
// import <string>;
// import <format>;
// import <chrono>;
// import <map>;

namespace p3
{
	namespace unit_test
	{
#pragma region basic types and concepts

		using result_type = bool;

		export
		struct basic_test
		{
			virtual void run_test() const = 0;
		};
		using test_ptr = std::unique_ptr<basic_test>;

		export
		struct basic_factory
		{
			virtual test_ptr create() const = 0;
		};
		using factory_ptr = std::unique_ptr<basic_factory>;

		std::map<std::string, std::unique_ptr<basic_factory>> private_factory_map;

#pragma endregion
#pragma region factory, inserter

		export
		template <typename test_type>
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

		export
		template <typename test_type>
		struct factory_inserter
		{
			factory_inserter()
			{
				// I know, rtti is not the best thing to use, but I think for testing purposes it's fine.
				// The performance itself is no problem, since we're pre-collecting all the names.
				std::string name = typeid(test_type).name();

				for (const std::string replacement : { "struct ", "class " })
				{
					for (auto found = name.find(replacement); found != std::string::npos; found = name.find(replacement, found))
					{
						name.replace(found, replacement.size(), "");
					}
				}

				std::clog << console::cyan.bright() << "registering test " << console::cyan.dark() << '"' << name << '"' << console::cyan.bright() << "..." << console::reset << std::endl;
				private_factory_map[name] = std::make_unique<factory<test_type>>(name);
			}
		};

#pragma endregion
#pragma region templated test cases

		export
		template <typename test_type>
		struct test_case : basic_test
		{
		private:
			// this is the hack. by adding a static factory_inserter to a test case, we can use the following line to invoke the registration of the test.
			static inline factory_inserter<test_type> s_inserter{};
		};

#pragma endregion
#pragma region actual test

		export
		std::map<std::string, result_type> run()
		{
			std::map<std::string, result_type> result;
			for (const auto &[name, creator] : private_factory_map)
			{
				std::cout << std::endl << "running test \"" << name << "\"..." << std::endl;
				const auto test_object = creator->create();

				const auto begin_time = std::chrono::system_clock::now();

				bool success = true;
				try
				{
					test_object->run_test();
				}
				catch (const std::exception &e)
				{
					success = false;
					std::cout << console::yellow.dark() << " -> " << e.what() << console::reset << std::endl;
				}
				catch (const char *msg)
				{
					success = false;
					std::cout << console::yellow.dark() << " -> " << msg << console::reset << std::endl;
				}
				catch (...)
				{
					success = false;
					std::cout << console::yellow.dark() << " -> Something not inheriting from std::exception has been thrown" << console::reset << std::endl;
				}

				const auto end_time = std::chrono::system_clock::now();
				const auto delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time);

				const auto color = success ? console::green : console::red;
				std::cout << color.bright() << " -> test " << color.dark() << '"' << name << '"' << color.bright() << ' ' << (success ? "succeeded" : "failed") << " (" << delta_time.count() << "ms)" << console::reset << std::endl;
				result[name] = success;
			}
			return result;
		}

#pragma endregion
#pragma region evaluation

		export
		void evaluate(std::size_t successful, std::size_t total)
		{
			std::cout << "Final unit test summary:" << std::endl;

			const double percentage = 100.0 * successful / total;
			std::cout << std::format(" -> Successful tests: {2:.2f}% ({0}/{1})", successful, total, percentage) << std::endl;

			const std::string summary = successful == total ? "You may push your code! :)" : "Do not upload this code! :(";
			const auto color = successful == total ? console::green.dark() : console::red.dark();
			std::cout << color << " -> " << summary << console::reset << std::endl;
		}

		export
		void evaluate(const std::map<std::string, result_type> &results)
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
	}
}