export module p3.meta.list.manipulators;

export import p3.meta.list.core;
import <concepts>;

/*
	Naming convention:

	type<list<>> spawns a second type_raw<typename...>
	direction of abstraction depends on which side is easier to implement...
*/


#pragma region custom keywords

#define predicate template <typename> typename
#define unary_op template <typename> typename
#define binary_op template <typename, typename> typename
#define container template <typename...> typename

#pragma endregion

namespace p3
{
	namespace meta
	{
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
