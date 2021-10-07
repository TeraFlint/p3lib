export module p3.meta.list.core;

import <concepts>;

// just a manual marker for specialized templates.
// the special highliting of macros should make it easier to catch a structure specialization.
#define specialize export

namespace p3
{
	namespace meta
	{
#pragma region basics

		template <size_t value>
		struct sized { static constexpr size_t size = value; };

		template <size_t value>
		using index_constant = std::integral_constant<size_t, value>;

		template <typename type>
		struct type_level_depth : index_constant<0> {};

		template <typename type>
		struct value_level_depth : index_constant<0> {};

		template <typename type, size_t depth>
		concept type_level = type_level_depth<type>::value == depth;

		template <typename type, size_t depth>
		concept value_level = value_level_depth<type>::value == depth;

#pragma endregion
#pragma region type list

		// level 0: type alias
		export
		template <typename content>
		using types_0 = content;

		// level 1: list of types
		export
		template <typename ...contents>
		struct types_1 : sized<sizeof...(contents)> {};

		specialize
		template <typename ...contents>
		struct type_level_depth<types_1<contents...>> : index_constant<1> {};

		export
		template <typename ...contents>
		using types = types_1<contents...>;

		// level 2: list of incomplete template types
		export
		template <template <typename ...> typename ...contents>
		struct types_2 : sized<sizeof...(contents)> {};

		specialize
		template <template <typename ...> typename ...contents>
		struct type_level_depth<types_2<contents...>> : index_constant<2> {};

		// higher ordered types_n here, if the need arises.

#pragma endregion
#pragma region value list

		// level 0: constant
		export
		template <auto content>
		constexpr auto values_0 = content;

		// level 1: list of values
		export
		template <auto ...contents>
		struct values_1 : sized<sizeof...(contents)> {};

		specialize
		template <auto ...contents>
		struct value_level_depth<values_1<contents...>> : index_constant<1> {};

		export
		template <auto ...contents>
		using values = values_1<contents...>;

		// level 2: list of incomplete template value lists
		export
		template <template <auto ...> typename ...contents>
		struct values_2 : sized<sizeof...(contents)> {};

		specialize
		template <template <auto ...> typename ...contents>
		struct value_level_depth<values_2<contents...>> : index_constant<2> {};

		// higher ordered values_n here, if the need arises.

#pragma endregion
#pragma region static_assert

		// index_constant and sized
		static_assert(index_constant<-1>::value > 0,       "p3::meta::index_constant<>: negative indices possible");
		static_assert(index_constant<0>::value == 0,       "p3::meta::index_constant<>: value unexpected");
		static_assert(index_constant<1337>::value == 1337, "p3::meta::index_constant<>: value unexpected");

		static_assert(sized<-1>::size > 0, "p3::meta::sized<>: negative sizes possible");
		static_assert(sized<0>::size == 0, "p3::meta::sized<>: value unexpected");
		static_assert(sized<5>::size == 5, "p3::meta::sized<>: value unexpected");

		// types
		static_assert(std::same_as<types_0<int>, int>,                    "p3::meta::types_0<>: expected int");
		static_assert(std::same_as<types_1<>, types<>>,                   "p3::meta::types<>: not an alias of types_1<>");
		static_assert(std::same_as<types_1<char, int>, types<char, int>>, "p3::meta::types<>: not an alias of types_1<>");

		static_assert(types_1<>::size == 0,                   "p3::meta::types_1<>: expected size 0");
		static_assert(types_1<bool, char, double>::size == 3, "p3::meta::types_1<>: expected size 3");
		static_assert(types_2<>::size == 0,                   "p3::meta::types_2<>: expected size 0");
		static_assert(types_2<types_1, types_1>::size == 2,   "p3::meta::types_2<>: expected size 2");

		// values
		static_assert(values_0<true> == true, "p3::meta::values_0<>: value unexpected");
		static_assert(values_0<80> == 80,     "p3::meta::values_0<>: value unexpected");
		static_assert(values_0<-7> == -7,     "p3::meta::values_0<>: value unexpected");
		static_assert(values_0<3.14> == 3.14, "p3::meta::values_0<>: value unexpected");

		static_assert(values_1<>::size == 0,         "p3::meta::values_1<>: expected size 0");
		static_assert(values_1<3>::size == 1,        "p3::meta::values_1<>: expected size 1");
		static_assert(values_1<'a', 0.5>::size == 2, "p3::meta::values_1<>: expected size 2");
		static_assert(values_2<>::size == 0,         "p3::meta::values_2<>: expected size 0");
		static_assert(values_2<values_1>::size == 1, "p3::meta::values_2<>: expected size 1");

		// type_level
		static_assert(type_level<int, 0>,                "p3::meta::type_level<>: expected level 0");
		static_assert(type_level<char, 0>,               "p3::meta::type_level<>: expected level 0");
		static_assert(type_level<types_1<>, 1>,          "p3::meta::type_level<>: expected level 1");
		static_assert(type_level<types_1<int>, 1>,       "p3::meta::type_level<>: expected level 1");
		static_assert(type_level<types_2<>, 2>,          "p3::meta::type_level<>: expected level 2");
		static_assert(type_level<types_2<types_1>, 2>,   "p3::meta::type_level<>: expected level 2");
		static_assert(type_level<values_1<>, 0>,         "p3::meta::type_level<>: expected level 0");
		static_assert(type_level<values_1<5>, 0>,        "p3::meta::type_level<>: expected level 0");
		static_assert(type_level<values_2<>, 0>,         "p3::meta::type_level<>: expected level 0");
		static_assert(type_level<values_2<values_1>, 0>, "p3::meta::type_level<>: expected level 0");

		// value_level
		static_assert(value_level<int, 0>,              "p3::meta::value_level<>: expected level 0");
		static_assert(value_level<char, 0>,             "p3::meta::value_level<>: expected level 0");
		static_assert(value_level<types_1<>, 0>,        "p3::meta::value_level<>: expected level 0");
		static_assert(value_level<types_1<int>, 0>,     "p3::meta::value_level<>: expected level 0");
		static_assert(value_level<types_2<>, 0>,        "p3::meta::value_level<>: expected level 0");
		static_assert(value_level<types_2<types_1>, 0>, "p3::meta::value_level<>: expected level 0");
		static_assert(value_level<values_1<>, 1>,       "p3::meta::value_level<>: expected level 1");
		static_assert(value_level<values_1<5>, 1>,      "p3::meta::value_level<>: expected level 1");
		static_assert(value_level<values_2<>, 2>,       "p3::meta::value_level<>: expected level 2");
		// static_assert(value_level<values_2<values_1>, 2>, "p3::meta::value_level<>: expected level 2");

#pragma endregion
	}
}