export module p3.meta.list.core;

import <concepts>;

namespace p3
{
	namespace meta
	{
#pragma region basics

		template <typename type>
		struct is_type_sequence : std::false_type {};

		template <typename type>
		struct is_value_sequence : std::false_type {};

		export
		template <typename type>
		concept type_sequence = is_type_sequence<type>::value;

		export
		template <typename type>
		concept value_sequence = is_value_sequence<type>::value;

		export
		template <typename type>
		concept any_sequence = type_sequence<type> || value_sequence<type>;

#pragma endregion
#pragma region lists

		template <size_t value>
		struct sized
		{
			static constexpr size_t size() 
			{
				return value; 
			} 
		};

		export
		template <typename ...types>
		struct type_list : sized<sizeof...(types)>
		{
			template <typename function_type>
			static constexpr void for_each_type(function_type &&func)
			{
				(func.template operator()<types>(), ...);
			}

			template <typename function_type, typename ...par_types>
			static constexpr void for_each_value(function_type &&func, par_types &&...par)
				requires(std::constructible_from<types, par_types...> && ...)
			{
				// it's not contents{ par... } on purpose, to be less strict and thus compatible with more types.
				// after all, the whole point of this is to work with as many types as possible.
				(func(types(par...)), ...);
			}
		};

		template <typename ...types>
		struct is_type_sequence<type_list<types...>> : std::true_type {};

		export
		template <auto ...values>
		struct value_list : sized<sizeof...(values)>
		{
			template <typename function_type, typename ...par_types>
			static constexpr void for_each_value(function_type &&func)
			{
				(func(values), ...);
			}
		};

		template <auto ...values>
		struct is_value_sequence<value_list<values...>> : std::true_type {};

#pragma endregion
#pragma region static_assert

		// sizes
		static_assert(sized<-1>::size() > 0, "p3::meta::sized<>: expected positive sizes");
		static_assert(sized<0>::size() == 0, "p3::meta::sized<>: value unexpected");
		static_assert(sized<5>::size() == 5, "p3::meta::sized<>: value unexpected");

		static_assert(type_list<>                  ::size() == 0, "p3::meta::type_list<>: expected size 0");
		static_assert(type_list<bool, char, double>::size() == 3, "p3::meta::type_list<>: expected size 3");

		static_assert(value_list<>        ::size() == 0, "p3::meta::value_list<>: expected size 0");
		static_assert(value_list<3>       ::size() == 1, "p3::meta::value_list<>: expected size 1");
		static_assert(value_list<'a', 0.7>::size() == 2, "p3::meta::value_list<>: expected size 2");

		// *_sequence concept
		static_assert(!type_sequence<int>,                   "p3::meta::type_sequence<>: expected int to fail");
		static_assert(!type_sequence<char>,                  "p3::meta::type_sequence<>: expected char to fail");
		static_assert( type_sequence<type_list<>>,           "p3::meta::type_sequence<>: expected type_list<> to pass");
		static_assert( type_sequence<type_list<int, char>>,  "p3::meta::type_sequence<>: expected type_list<int, char> to pass");
		static_assert(!type_sequence<value_list<>>,          "p3::meta::type_sequence<>: expected value_list<> to fail");
		static_assert(!type_sequence<value_list<5, 'x'>>,    "p3::meta::type_sequence<>: expected value_list<5, 'x'> to fail");

		static_assert(!value_sequence<int>,                  "p3::meta::value_sequence<>: expected int to fail");
		static_assert(!value_sequence<char>,                 "p3::meta::value_sequence<>: expected char to fail");
		static_assert(!value_sequence<type_list<>>,          "p3::meta::value_sequence<>: expected type_list<> to fail");
		static_assert(!value_sequence<type_list<int, char>>, "p3::meta::value_sequence<>: expected type_list<int, char> to fail");
		static_assert( value_sequence<value_list<>>,         "p3::meta::value_sequence<>: expected value_list<> to pass");
		static_assert( value_sequence<value_list<5, 'x'>>,   "p3::meta::value_sequence<>: expected value_list<5, 'x'> to pass");

		static_assert(!any_sequence<int>,                    "p3::meta::any_sequence<>: expected int to fail");
		static_assert(!any_sequence<char>,                   "p3::meta::any_sequence<>: expected char to fail");
		static_assert( any_sequence<type_list<>>,            "p3::meta::any_sequence<>: expected type_list<> to pass");
		static_assert( any_sequence<type_list<int, char>>,   "p3::meta::any_sequence<>: expected type_list<int, char> to pass");
		static_assert( any_sequence<value_list<>>,           "p3::meta::any_sequence<>: expected value_list<> to pass");
		static_assert( any_sequence<value_list<5, 'x'>>,     "p3::meta::any_sequence<>: expected value_list<5, 'x'> to pass");

#pragma endregion
	}
}