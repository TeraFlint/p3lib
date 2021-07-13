export module p3.grid;
import <vector>;
import <array>;

// import <functional>;
// import <algorithm>;
// import <numeric>;

// here's a problem: intellisense is not happy with these imports. If any of them is imported, I get:
// "there are too many errors for the intellisense engine to function properly".
// the problem is: I'd rather have intellisense features during development than getting shorter code.
// so I will provide the oneliners, but add a todo comment and do the manual implementation. That way I still get support from the IDE.
// once the classes are finished, I add the imports it and swap out the manual implementations.
// thanks to the added unit tests, I'll then be able to see if it broke or not.

namespace p3
{
#pragma region nested list

	/*
	template <typename data_type, size_t dim>
	struct nested_list
	{
		using type = std::initializer_list<typename nested_list<data_type, dim - 1>::type>;
	};

	template <typename data_type>
	struct nested_list<data_type, 0>
	{
		using type = data_type;
	};

	export
	template <typename data_type, size_t dimensions>
	using nested_list_t = typename nested_list<data_type, dimensions>::type;
	*/

#pragma endregion
#pragma region grid size

	export
	template <size_t dim>
	class grid_size : public std::array<size_t, dim>
	{
	public:

		constexpr void fix_zeroes()
		{
			// todo: import <algorithm>;
			// std::transform(this->begin(), this->end(), this->begin(), [](auto elem) { return std::max(1U, elem); });
			for (auto &elem : *this)
			{
				if (!elem)
				{
					elem = 1;
				}
			}
		}

		[[nodiscard]] constexpr size_t elements() const
		{
			// todo: import <numeric>; import <functional>;
			// return std::accumulate(this->begin(), this->end(), 1, std::multiplies{});
			size_t result = 1;
			for (const auto &item : *this)
			{
				result *= item;
			}
			return result;
		}

		[[nodiscard]] constexpr size_t index_of(const grid_size<dim> &position) const
		{
			size_t result = 0, layer_size = 1;
			dual_iteration(this->rbegin(), position.rbegin(), [&](size_t size, size_t pos)
			{
				result += layer_size * pos;
				layer_size *= size;
			});
			return result;
		}
		[[nodiscard]] constexpr size_t index_in(const grid_size<dim> &boundary) const
		{
			return boundary.index_of(*this);
		}

		[[nodiscard]] static constexpr grid_size<dim> from_index(size_t index, const grid_size<dim> &boundary)
		{
			grid_size<dim> result{};
			dual_iteration(boundary.rbegin(), result.rbegin(), [&](size_t size, size_t &pos)
			{
				pos = index % size;
				index /= size;
			});
			return result;
		}

		[[nodiscard]] constexpr grid_size<dim> fit_to_data(size_t needed_elements, size_t axis = 0) const
		{
			auto result = *this;
			result.fix_zeroes();

			if (result.elements() != needed_elements)
			{
				const auto subgrid_size = result.elements() / result[axis];
				const auto remainder = needed_elements % subgrid_size;
				result[axis] = needed_elements / subgrid_size + (remainder > 0);
			}
			return result;
		}

	private:
		template <typename iter1_type, typename iter2_type, typename function_type>
		static constexpr void dual_iteration(iter1_type iter1, iter2_type iter2, const function_type &callback)
		{
			for (size_t i = 0; i < dim; ++i, ++iter1, ++iter2)
			{
				callback(*iter1, *iter2);
			}
		}
	};

#pragma endregion
#pragma region grid position

	export
	template <size_t dim>
	class grid_pos
	{
	public:

		// first, last, next
		// pos(), size()

	private:
		grid_size<dim> m_pos, m_size;
	};

#pragma endregion
#pragma region grid

	export
	template <typename data_type, size_t dim>
	class grid
	{
	public:
#pragma region constructors

		// default
		grid() = default;

		// implicit nested list constructor
		// grid(const nested_list_t<data_type, dim> &list);

		// size constructor
		explicit grid(const grid_size<dim> &size)
			: m_size(size), m_data(m_size.elements())
		{
		}

		// size + list constructor
		explicit grid(const grid_size<dim> &size, const std::initializer_list<data_type> &init)
			: m_size(size.fit_to_data(init.size()))
		{
			const auto elements = m_size.elements();
			m_data.reserve(elements);

			// todo: import <algorithm>;
			// std::copy(init.begin(), init.end(), std::back_inserter(m_data));
			for (const auto &item : init)
			{
				m_data.push_back(item);
			}

			// grow into the allocated space and fill the rest with default objects
			m_data.resize(elements);
		}

		// size + generator constructor
		// template <grid_modifier generator_type>
		// explicit grid(const grid_size<dim> &size, const generator_type &generator);


		// grid + converter constructor
		// template <grid_modifier converter_type>
		// explicit grid(const grid<data_type, dim> &other, const converter_type &converter);


#pragma endregion
#pragma region meta data

		[[nodiscard]] constexpr size_t rank() const
		{
			return dim;
		}

		[[nodiscard]] constexpr grid_size<dim> dimensions() const
		{
			return m_size;
		}

		[[nodiscard]] constexpr size_t dimension(size_t axis) const
		{
			return m_size[axis];
		}

		[[nodiscard]] constexpr size_t size() const
		{
			return m_data.size();
		}

#pragma endregion
#pragma region accessors and iterators

		[[nodiscard]] data_type &operator[](size_t index)
		{
			return m_data[index];
		}

		[[nodiscard]] const data_type &operator[](size_t index) const
		{
			return m_data[index];
		}

		[[nodiscard]] data_type &at(const grid_size<dim> &pos)
		{
			return m_data[m_size.index_of(pos)];
		}

		[[nodiscard]] const data_type &at(const grid_size<dim> &pos) const
		{
			return m_data[m_size.index_of(pos)];
		}


		// iterators

#pragma endregion
#pragma region partitons (subgrid, slice)

		// subgrid (single layer), slice (vector of layers)

#pragma endregion

		// kernel shennanigans?

	private:
		grid_size<dim> m_size;
		std::vector<data_type> m_data;
	};

#pragma endregion
}