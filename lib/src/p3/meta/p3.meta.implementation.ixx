export module p3.meta.implementation;

import <concepts>;

#define specialize export
#define specialize_hidden

/*
	This system is meant as meta information for the implementation state of a type:

	class foo
	{
	public:
		static constexpr auto implementation_state()
		{
		#if defined (_WIN32)
			// unfortunately, this class uses windows funtionality.
			return impl_state::full;
		#else
			// maybe I'll implement it once I'll figure out how to do it on other platforms
			return impl_state::unplanned;
		#endif
		}
	}

	struct bar
	{
		static constexpr auto implementation_state()
		{
			// currently not implemented, just there so the program compiles
			return impl_state::planned;
		}
	}
*/

namespace p3
{
	namespace meta
	{
		export
		enum class impl_state { unplanned, planned, partial, full };

		export
		template <typename type>
		concept impl_specified = requires(type obj)
		{
			{ type::implementation_state() } -> std::convertible_to<impl_state>;
		};

		export
		template <typename type>
		[[nodiscard]] constexpr impl_state implementation()
		{
			// a type is assumed complete unless stated otherwise.
			if constexpr (impl_specified<type>)
			{
				return type::implementation_state();
			}
			else
			{
				return impl_state::full;
			}
		}

		export
		template <typename type>
		[[nodicard]] constexpr impl_state implementation(type&& obj)
		{
			// implementation states are states of whole classes, not just objects. change my mind.
			return implementation<type>();
		}


		export
		template <typename type, impl_state state>
		concept impl_exact = implementation<type>() == state;

		export
		template <typename type, impl_state state>
		concept impl_min = static_cast<int>(implementation<type>()) >= static_cast<int>(state);

		export
		template <typename first, typename ...rest>
		concept impl_full = (impl_min<first, impl_state::full> && ... && impl_min<rest, impl_state::full>);

#pragma region static_assert

		template <size_t>
		struct test {};

		template <impl_state state = impl_state::full>
		struct impl_test_type 
		{
			static constexpr auto implementation_state() { return state; }
		};

		// making sure impl_test_type is behaving correctly
		static_assert(implementation<impl_test_type<impl_state::full>>()      == impl_state::full,      "p3::meta::implementation<>: expected full");
		static_assert(implementation<impl_test_type<impl_state::partial>>()   == impl_state::partial,   "p3::meta::implementation<>: expected partial");
		static_assert(implementation<impl_test_type<impl_state::planned>>()   == impl_state::planned,   "p3::meta::implementation<>: expected planned");
		static_assert(implementation<impl_test_type<impl_state::unplanned>>() == impl_state::unplanned, "p3::meta::implementation<>: expected unplanned");

		// impl_exact with all combinations
		static_assert(impl_exact<impl_test_type<impl_state::full>, impl_state::full>,           "p3::meta::implementation<>: expected full");
		static_assert(impl_exact<impl_test_type<impl_state::partial>, impl_state::partial>,     "p3::meta::implementation<>: expected partial");
		static_assert(impl_exact<impl_test_type<impl_state::planned>, impl_state::planned>,     "p3::meta::implementation<>: expected planned");
		static_assert(impl_exact<impl_test_type<impl_state::unplanned>, impl_state::unplanned>, "p3::meta::implementation<>: expected unplanned");

		// impl_min with all combinations
		static_assert( impl_min<impl_test_type<impl_state::full>,      impl_state::unplanned>, "p3::meta::implementation<>: expected >= unplanned");
		static_assert( impl_min<impl_test_type<impl_state::partial>,   impl_state::unplanned>, "p3::meta::implementation<>: expected >= unplanned");
		static_assert( impl_min<impl_test_type<impl_state::planned>,   impl_state::unplanned>, "p3::meta::implementation<>: expected >= unplanned");
		static_assert( impl_min<impl_test_type<impl_state::unplanned>, impl_state::unplanned>, "p3::meta::implementation<>: expected >= unplanned");
		static_assert( impl_min<impl_test_type<impl_state::full>,      impl_state::planned>,   "p3::meta::implementation<>: expected >= planned");
		static_assert( impl_min<impl_test_type<impl_state::partial>,   impl_state::planned>,   "p3::meta::implementation<>: expected >= planned");
		static_assert( impl_min<impl_test_type<impl_state::planned>,   impl_state::planned>,   "p3::meta::implementation<>: expected >= planned");
		static_assert(!impl_min<impl_test_type<impl_state::unplanned>, impl_state::planned>,   "p3::meta::implementation<>: expected < planned");
		static_assert( impl_min<impl_test_type<impl_state::full>,      impl_state::partial>,   "p3::meta::implementation<>: expected >= partial");
		static_assert( impl_min<impl_test_type<impl_state::partial>,   impl_state::partial>,   "p3::meta::implementation<>: expected >= partial");
		static_assert(!impl_min<impl_test_type<impl_state::planned>,   impl_state::partial>,   "p3::meta::implementation<>: expected < partial");
		static_assert(!impl_min<impl_test_type<impl_state::unplanned>, impl_state::partial>,   "p3::meta::implementation<>: expected < partial");
		static_assert( impl_min<impl_test_type<impl_state::full>,      impl_state::full>,      "p3::meta::implementation<>: expected >= full");
		static_assert(!impl_min<impl_test_type<impl_state::partial>,   impl_state::full>,      "p3::meta::implementation<>: expected < full");
		static_assert(!impl_min<impl_test_type<impl_state::planned>,   impl_state::full>,      "p3::meta::implementation<>: expected < full");
		static_assert(!impl_min<impl_test_type<impl_state::unplanned>, impl_state::full>,      "p3::meta::implementation<>: expected < full");

		// impl_full with all single cases
		static_assert( impl_full<impl_test_type<impl_state::full>>,      "p3::meta::implementation<>: expected full");
		static_assert(!impl_full<impl_test_type<impl_state::partial>>,   "p3::meta::implementation<>: expected not full");
		static_assert(!impl_full<impl_test_type<impl_state::planned>>,   "p3::meta::implementation<>: expected not full");
		static_assert(!impl_full<impl_test_type<impl_state::unplanned>>, "p3::meta::implementation<>: expected not full");

		// and some double cases
		static_assert( impl_full<impl_test_type<impl_state::full>,    impl_test_type<impl_state::full>>,    "p3::meta::implementation<>: expected full");
		static_assert(!impl_full<impl_test_type<impl_state::full>,    impl_test_type<impl_state::partial>>, "p3::meta::implementation<>: expected not full");
		static_assert(!impl_full<impl_test_type<impl_state::partial>, impl_test_type<impl_state::full>>,    "p3::meta::implementation<>: expected not full");
		static_assert(!impl_full<impl_test_type<impl_state::partial>, impl_test_type<impl_state::partial>>, "p3::meta::implementation<>: expected not full");

		// behavior of other types
		static_assert(impl_full<int>,            "p3::meta::implementation<>: expected full");
		static_assert(impl_full<char>,           "p3::meta::implementation<>: expected full");
		static_assert(impl_full<void>,           "p3::meta::implementation<>: expected full");
		static_assert(impl_full<impl_state>,     "p3::meta::implementation<>: expected full");
		static_assert(impl_full<std::true_type>, "p3::meta::implementation<>: expected full");

#pragma endregion
	}
}