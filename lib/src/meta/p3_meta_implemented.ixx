export module p3.meta.implemented;

import <concepts>;

#define specialize_hidden

namespace p3
{
	namespace meta
	{
		export
		enum class impl_state { unplanned, planned, partial, full };

		export
		template <impl_state value>
		struct impl_base
		{
			static constexpr impl_state state = value;
		};

		export
		template <typename type>
		struct implementation : impl_base<impl_state::full> {};

		export
		template <typename type, impl_state state>
		concept impl_exact = implementation<type>::state == state;

		export
		template <typename type, impl_state state>
		concept impl_min = static_cast<int>(implementation<type>::state) >= static_cast<int>(state);

		export
		template <typename first, typename ...types>
		concept impl_full = (impl_min<first, impl_state::full> && ... && impl_min<types, impl_state::full>);

#pragma region static_assert

		template <size_t>
		struct test {};

		template <impl_state value = impl_state::full>
		struct impl_test_type {};

		specialize_hidden
		template <impl_state value>
		struct implementation<impl_test_type<value>> : impl_base<value> {};

		// making sure impl_test_type is behaving correctly
		static_assert(implementation<impl_test_type<impl_state::full>>     ::state == impl_state::full,      "p3::meta::implementation<>: expected full");
		static_assert(implementation<impl_test_type<impl_state::partial>>  ::state == impl_state::partial,   "p3::meta::implementation<>: expected partial");
		static_assert(implementation<impl_test_type<impl_state::planned>>  ::state == impl_state::planned,   "p3::meta::implementation<>: expected planned");
		static_assert(implementation<impl_test_type<impl_state::unplanned>>::state == impl_state::unplanned, "p3::meta::implementation<>: expected unplanned");

		// impl_exact with all combinations
		static_assert(impl_exact<impl_test_type<impl_state::full>, impl_state::full>,           "p3::meta::implementation<>: expected full");
		static_assert(impl_exact<impl_test_type<impl_state::partial>, impl_state::partial>,     "p3::meta::implementation<>: expected partial");
		static_assert(impl_exact<impl_test_type<impl_state::planned>, impl_state::planned>,     "p3::meta::implementation<>: expected planned");
		static_assert(impl_exact<impl_test_type<impl_state::unplanned>, impl_state::unplanned>, "p3::meta::implementation<>: expected unplanned");

		// impl_min with all combinations
		static_assert(impl_min<impl_test_type<impl_state::full>, impl_state::unplanned>,      "p3::meta::implementation<>: expected >= unplanned");
		static_assert(impl_min<impl_test_type<impl_state::partial>, impl_state::unplanned>,   "p3::meta::implementation<>: expected >= unplanned");
		static_assert(impl_min<impl_test_type<impl_state::planned>, impl_state::unplanned>,   "p3::meta::implementation<>: expected >= unplanned");
		static_assert(impl_min<impl_test_type<impl_state::unplanned>, impl_state::unplanned>, "p3::meta::implementation<>: expected >= unplanned");
		static_assert(impl_min<impl_test_type<impl_state::full>, impl_state::planned>,        "p3::meta::implementation<>: expected >= planned");
		static_assert(impl_min<impl_test_type<impl_state::partial>, impl_state::planned>,     "p3::meta::implementation<>: expected >= planned");
		static_assert(impl_min<impl_test_type<impl_state::planned>, impl_state::planned>,     "p3::meta::implementation<>: expected >= planned");
		static_assert(!impl_min<impl_test_type<impl_state::unplanned>, impl_state::planned>,  "p3::meta::implementation<>: expected < planned");
		static_assert(impl_min<impl_test_type<impl_state::full>, impl_state::partial>,        "p3::meta::implementation<>: expected >= partial");
		static_assert(impl_min<impl_test_type<impl_state::partial>, impl_state::partial>,     "p3::meta::implementation<>: expected >= partial");
		static_assert(!impl_min<impl_test_type<impl_state::planned>, impl_state::partial>,    "p3::meta::implementation<>: expected < partial");
		static_assert(!impl_min<impl_test_type<impl_state::unplanned>, impl_state::partial>,  "p3::meta::implementation<>: expected < partial");
		static_assert(impl_min<impl_test_type<impl_state::full>, impl_state::full>,           "p3::meta::implementation<>: expected >= full");
		static_assert(!impl_min<impl_test_type<impl_state::partial>, impl_state::full>,       "p3::meta::implementation<>: expected < full");
		static_assert(!impl_min<impl_test_type<impl_state::planned>, impl_state::full>,       "p3::meta::implementation<>: expected < full");
		static_assert(!impl_min<impl_test_type<impl_state::unplanned>, impl_state::full>,     "p3::meta::implementation<>: expected < full");

		// impl_full with all single cases
		static_assert(impl_full<impl_test_type<impl_state::full>>,       "p3::meta::implementation<>: expected full");
		static_assert(!impl_full<impl_test_type<impl_state::partial>>,   "p3::meta::implementation<>: expected not full");
		static_assert(!impl_full<impl_test_type<impl_state::planned>>,   "p3::meta::implementation<>: expected not full");
		static_assert(!impl_full<impl_test_type<impl_state::unplanned>>, "p3::meta::implementation<>: expected not full");

		// and some double cases
		static_assert(impl_full<impl_test_type<impl_state::full>, impl_test_type<impl_state::full>>,        "p3::meta::implementation<>: expected full");
		static_assert(!impl_full<impl_test_type<impl_state::full>, impl_test_type<impl_state::partial>>,    "p3::meta::implementation<>: expected not full");
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