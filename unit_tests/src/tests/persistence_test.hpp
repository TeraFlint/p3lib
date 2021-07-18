#pragma once
#include "../unit_test.hpp"
import p3.persistence;

namespace file_access_test
{
	class file_access_impl : public p3::file_access
	{
	public:
		constexpr file_access_impl(bool load, bool save, const std::string &name = {})
			: m_load(load), m_save(save), m_name(name)
		{
		}

		constexpr auto operator<=>(const file_access_impl &other) const = default;

	private:
		bool m_load, m_save;
		std::string m_name;

	protected:
		bool on_load(const path_type &location) override
		{
			std::cout << "loading " << m_name << "... " << m_load << std::endl;
			return m_load;
		}
		bool on_save(const path_type &location) const override
		{
			std::cout << "saving  " << m_name << "... " << m_save << std::endl;
			return m_save;
		}
	};

	template <typename function_type>
	void assert_exception(bool expected, function_type function, const std::string &name)
	{
		bool thrown = false;
		try { function(); }
		catch (...) { thrown = true; }
		if (thrown != expected)
		{
			const std::string message = expected ? " was supposed to throw, but did not" : " was supposed to not throw, but did";
			throw std::runtime_error("assert " + name + message);
		}
	}

	void test_exceptions(bool load, bool save, const std::string &name, const p3::file_access::path_type location = "file")
	{
		file_access_impl dummy(load, save, name);
		assert_exception(!load, [&]() { dummy.load(location); }, name + " (load)");
		assert_exception(!save, [&]() { dummy.save(location); }, name + " (save)");
	}

}

#pragma region file_access

P3_UNIT_TEST(file_access_exception_all_success)
{
	file_access_test::test_exceptions( true,  true, " all_success");
}

P3_UNIT_TEST(file_access_exception_load_success)
{
	file_access_test::test_exceptions( true, false, "load_success");
}

P3_UNIT_TEST(file_access_exception_save_success)
{
	file_access_test::test_exceptions(false,  true, "save_success");
}

P3_UNIT_TEST(file_access_exception_no_success)
{
	file_access_test::test_exceptions(false, false, "  no_success");
}

#pragma endregion