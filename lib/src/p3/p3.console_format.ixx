#include <concepts>
#include <iostream>
#include <format>
export module p3.console_format;

namespace p3
{
	namespace console_format
	{
#pragma region base structs

		/*
			A basic multi-purpose arbitrary value wrapper for inheritance.
			Overloads for this struct are forbidden.
		*/
		export
		template <typename data_type>
		struct basic_wrapper
		{
		public:
			constexpr basic_wrapper() = default;
			constexpr explicit basic_wrapper(data_type &&value) : m_value{ std::move(value) } {}
			constexpr explicit basic_wrapper(const data_type &value) : m_value{ value } {}

			constexpr const data_type &value() const { return m_value; }
			constexpr explicit operator bool() const { return m_value; }

		protected:
			data_type m_value{};
		};

#pragma endregion
#pragma region signals

		export
		template <std::integral data_type>
		struct basic_signal
			: basic_wrapper<data_type>
		{
		public:
			using basic_wrapper<data_type>::basic_wrapper;

			template <template <typename> typename desired_wrapper_type>
			constexpr desired_wrapper_type<data_type> convert_to(const data_type &offset = 0) const
			{
				return desired_wrapper_type<data_type>{ static_cast<data_type>(this->m_value + offset) };
			}

			friend std::ostream &operator<<(std::ostream &out, const basic_signal<data_type> &item)
			{
				return out << std::format("\x1B[{}m", item.value());
			}
		};

		export
		template <std::integral data_type>
		struct basic_deep_color
			: basic_signal<data_type>
		{
			using basic_signal<data_type>::basic_signal;
			constexpr basic_signal<data_type> foreground() const { return this->convert_to<basic_signal>(0); }
			constexpr basic_signal<data_type> background() const { return this->convert_to<basic_signal>(10); }
		};

		export
		template <std::integral data_type>
		struct basic_shallow_color
			: basic_signal<data_type>
		{
			using basic_signal<data_type>::basic_signal;
			constexpr basic_deep_color<data_type> dark()   const { return this->convert_to<basic_deep_color>(0); }
			constexpr basic_deep_color<data_type> bright() const { return this->convert_to<basic_deep_color>(60); }
		};

		export
		template <std::integral data_type>
		struct basic_decoration
			: basic_signal<data_type>
		{
			using basic_signal<data_type>::basic_signal;
			constexpr basic_signal<data_type> enable()  const { return this->convert_to<basic_signal>(0); }
			constexpr basic_signal<data_type> disable() const { return this->convert_to<basic_signal>(20); }
		};

		export
		template <typename index_type>
		struct basic_constants
		{
			using signal        = basic_signal<index_type>;
			using decoration    = basic_decoration<index_type>;
			using deep_color    = basic_deep_color<index_type>;
			using shallow_color = basic_shallow_color<index_type>;

			static constexpr signal reset{ 0 };
			static constexpr decoration bold{ 1 }, underline{ 4 }, negative{ 7 };
			static constexpr shallow_color black{ 30 }, red{ 31 }, green{ 32 }, yellow{ 33 }, blue{ 34 }, magenta{ 35 }, cyan{ 36 }, gray{ 37 };
		};

#pragma endregion
	}

	export
	using console = console_format::basic_constants<unsigned char>;
}