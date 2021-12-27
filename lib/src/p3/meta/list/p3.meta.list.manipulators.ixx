export module p3.meta.list.manipulators;

export import p3.meta.list.core;
import <concepts>;

/*
	Naming convention:

	foo<types<...>> spawns a second foo_raw<...>
	direction of abstraction depends on which side is easier to implement...

	note to self: if herb sutter's meta class project makes its way into c++ in the future,
	these extra default implementations might be generatable classes from the implementation.

	for now, to_*_manipulator has to do the trick.
*/


#pragma region custom keywords

#define predicate template <typename> typename
#define unary_op  template <typename> typename
#define binary_op template <typename, typename> typename
#define container template <typename...> typename

#pragma endregion

namespace p3
{
	namespace meta
	{
		// template <typename ...pack>
		// using action_raw = to_pack_manipulator<action, pack...>;

		template <template <typename> typename list_manipulator, typename ...pack>
		struct to_pack_manipulator
		{
			using type = list_manipulator<types<pack...>>::type;
		};

		// template <typename list>
		// using action = to_list_manipulator<action_raw, list>;

		template <template <typename...> typename pack_manipulator, typename list>
		struct to_list_manipulator {};

		template <template <typename...> typename pack_manipulator, typename ...contents>
		struct to_list_manipulator<pack_manipulator, types<contents...>>
		{
			using type = pack_manipulator<contents...>::type;
		};



		// just forwards the input type. saves a type declaration in the body.
		// potentially the most used struct in the long term.
		export
		template <typename data_type>
		struct forward { using type = data_type; };



		// this is where the fun begins!



#pragma region static_assert

		static_assert(std::same_as<forward<int>::type, int>, "p3::meta::forward<>: expected int");
		static_assert(std::same_as<forward<forward<char>>::type, forward<char>>, "p3::meta::forward<>: expected forward");
		static_assert(std::same_as<forward<forward<char>::type>::type, char>, "p3::meta::forward<>: expected char");

#pragma endregion

	}
}
