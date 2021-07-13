export module p3.grid;
import <vector>;
import <array>;

// import <functional>;
// import <algorithm>;
// import <numeric>;

// here's a problem: intellisense is not happy with these imports. If any of them is imported, I get:
// "E2996: There are too many errors for the IntelliSense engine to function properly".
// the problem is: I'd rather have intellisense features during development than getting shorter code.
// so I will provide the oneliners, but add a todo comment and do the manual implementation. That way I still get support from the IDE.
// once the classes are finished, I add the imports it and swap out the manual implementations.
// thanks to the added unit tests, I'll then be able to see if it broke or not.

namespace p3
{
#pragma region helper functions (not supposed to be exported)

	template <typename iter1_type, typename iter2_type, typename function_type>
	static constexpr bool dual_iteration_n(size_t iterations, iter1_type iter1, iter2_type iter2, const function_type &callback)
	{
		for (size_t i = 0; i < iterations; ++i, ++iter1, ++iter2)
		{
			if (!callback(*iter1, *iter2))
			{
				return false;
			}
		}
		return true;
	}

#pragma endregion
#pragma region nested list

	/*
	template <typename data_type, size_t dimensions>
	struct nested_list
	{
		using type = std::initializer_list<typename nested_list<data_type, dimensions - 1>::type>;
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
	template <size_t dimensions>
	class grid_size : public std::array<size_t, dimensions>
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

		[[nodiscard]] constexpr size_t index_of(const grid_size<dimensions> &position) const
		{
			size_t result = 0, layer_size = 1;
			dual_iteration_n(dimensions, this->rbegin(), position.rbegin(), [&](size_t size, size_t pos)
			{
				result += layer_size * pos;
				layer_size *= size;
				return true;
			});
			return result;
		}

		[[nodiscard]] constexpr size_t index_in(const grid_size<dimensions> &boundary) const
		{
			return boundary.index_of(*this);
		}

		[[nodiscard]] static constexpr grid_size<dimensions> from_index(size_t index, const grid_size<dimensions> &boundary)
		{
			grid_size<dimensions> result{};
			dual_iteration_n(dimensions, boundary.rbegin(), result.rbegin(), [&](size_t size, size_t &pos)
			{
				pos = index % size;
				index /= size;
				return true;
			});
			return result;
		}

		[[nodiscard]] constexpr grid_size<dimensions> fit_to_data(size_t needed_elements, size_t axis = 0) const
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
	};

#pragma endregion
#pragma region grid position

	export
	template <size_t dimensions>
	class grid_pos
	{
	#pragma region constructors

	public:
		constexpr explicit grid_pos(const grid_size<dimensions> &size = {})
			: m_dim{ size }
		{
		}

	#pragma endregion
	#pragma region accessors

	public:
		constexpr const grid_size<dimensions> &dim() const
		{
			return m_dim;
		}
		constexpr const grid_size<dimensions> &pos() const
		{
			return m_pos;
		}

		constexpr const size_t &dim_at(const size_t axis) const
		{
			return m_dim[axis];
		}
		constexpr const size_t &pos_at(const size_t axis) const
		{
			return m_pos[axis];
		}

		constexpr bool valid() const
		{
			return m_valid;
		}

	#pragma endregion
	#pragma region meta data

	public:
		constexpr size_t index() const
		{
			return m_dim.index_of(m_pos);
		}

		// todo: operator<=>()
		constexpr bool operator==(const grid_pos<dimensions> &other) const
		{
			return m_dim == other.m_dim && m_pos == other.m_pos;
		}

	#pragma endregion
	#pragma region manipulators

	public:
		constexpr void first()
		{
			m_pos = {};
		}

		constexpr void last()
		{
			// todo: import <algorithm>;
			// std::transform(m_dim.begin(), m_dim.end(), m_pos.begin(), [](auto x) { return x > 0 ? x - 1 : 0; });

			auto iter = m_pos.begin();
			for (const auto &axis : m_dim)
			{
				*iter = axis ? axis - 1 : 0;
				++iter;
			}
		}

		constexpr bool jump(const grid_size<dimensions> &pos)
		{
			const auto check = [](auto dim, auto pos) { return pos < dim; };
			const bool in_bounds = dual_iteration_n(dimensions, m_dim.begin(), pos.begin(), check);
			if (in_bounds)
			{
				m_pos = pos;
			}
			return in_bounds;
		}

		constexpr bool next()
		{
			const auto can_terminate = [](const auto &dim, auto &pos) { return pos + 1 < dim; };
			const auto operation     = [](const auto &dim, auto &pos) { ++pos; };
			const auto reset         = [](const auto &dim, auto &pos) { pos = 0; };
			return iterate_backwards_until(can_terminate, operation, reset);
		}

		constexpr bool prev()
		{
			const auto can_terminate = [](const auto &dim, auto &pos) { return pos > 0; };
			const auto operation     = [](const auto &dim, auto &pos) { --pos; };
			const auto reset         = [](const auto &dim, auto &pos) { pos = dim - 1; };
			return iterate_backwards_until(can_terminate, operation, reset);
		}

	private:
		template <typename check_type, typename operation_type, typename reset_type>
		constexpr bool iterate_backwards_until(const check_type &can_terminate, const operation_type &operation, const reset_type &reset)
		{
			auto dim_iter = m_dim.crbegin();
			auto pos_iter = m_pos.rbegin();
			for (size_t i = 0; i < dimensions; ++i, ++dim_iter, ++pos_iter)
			{
				// do the operation if you can and report success
				if (can_terminate(*dim_iter, *pos_iter))
				{
					operation(*dim_iter, *pos_iter);
					return true;
				}
				// otherwise reset and carry (a.k.a. retry in the next iteration)
				reset(*dim_iter, *pos_iter);
			}
			return m_valid = false;
		}

	#pragma endregion
	#pragma region operators

	public:
		constexpr grid_pos<dimensions> &operator++()
		{
			next();
			return *this;
		}

		constexpr grid_pos<dimensions> operator++(int)
		{
			auto copy = *this;
			next();
			return copy;
		}

		constexpr grid_pos<dimensions> &operator--()
		{
			prev();
			return *this;
		}

		constexpr grid_pos<dimensions> operator--(int)
		{
			auto copy = *this;
			prev();
			return copy;
		}

	#pragma endregion
	#pragma region member variables

	private:
		grid_size<dimensions> m_dim{}, m_pos{};
		bool m_valid = true;

	#pragma endregion
	};

#pragma endregion
#pragma region grid modifier concept

	template <typename data_type>
	concept any_type = true;

	/*
	* This concept makes sure it receives a function-like type capable of accepting a grid_pos.
	* The return type doesn't matter as all the algorithms using it discard it.
	* So void functions would make sense, but I'd like to stay compatible to other functions, as well.
	*/
	export
	template <typename function_type, size_t dim>
	concept grid_modifier = requires(function_type function)
	{
		{ function(grid_pos<dim>{}) } -> any_type;
	};

#pragma endregion
#pragma region grid

	export
	template <typename data_type, size_t dimensions>
	class grid
	{
#pragma region constructors

	public:
		// default
		grid() = default;

		// implicit nested list constructor
		// grid(const nested_list_t<data_type, dimensions> &list);

		// size constructor
		explicit grid(const grid_size<dimensions> &size)
			: m_dim{ size }, m_data(m_dim.elements())
		{
		}

		// size + list constructor
		explicit grid(const grid_size<dimensions> &size, const std::initializer_list<data_type> &init)
			: m_dim{ size.fit_to_data(init.size()) }
		{
			reserve_fill_resize([&]()
			{
				// todo: import <algorithm>;
				// std::copy(init.begin(), init.end(), std::back_inserter(m_data));
				for (const auto &item : init)
				{
					m_data.push_back(item);
				}
			});
		}

		// size + generator constructor
		template <grid_modifier<dimensions> generator_type>
		explicit grid(const grid_size<dimensions> &size, const generator_type &generator)
			: m_dim{ size }
		{
			// todo
		}


		// grid + converter constructor
		// template <grid_modifier<dimensions> converter_type>
		// explicit grid(const grid<data_type, dimensions> &other, const converter_type &converter);

	private:
		template <typename function_type>
		void reserve_fill_resize(const function_type &fill)
		{
			// performs one single allocation and grows into the space (with default objects) in the end.
			const auto elements = m_dim.elements();
			m_data.reserve(elements);
			fill();
			m_data.resize(elements);
		}

	#pragma endregion
	#pragma region meta data

	public:
		[[nodiscard]] constexpr size_t rank() const
		{
			return dimensions;
		}

		[[nodiscard]] constexpr grid_size<dimensions> dim() const
		{
			return m_dim;
		}

		[[nodiscard]] constexpr size_t dim_at(size_t axis) const
		{
			return m_dim[axis];
		}

		[[nodiscard]] constexpr size_t size() const
		{
			return m_data.size();
		}

	#pragma endregion
	#pragma region accessors and iterators

	public:
		[[nodiscard]] data_type &operator[](size_t index)
		{
			return m_data[index];
		}

		[[nodiscard]] const data_type &operator[](size_t index) const
		{
			return m_data[index];
		}

		[[nodiscard]] data_type &at(const grid_size<dimensions> &pos)
		{
			return m_data[m_dim.index_of(pos)];
		}

		[[nodiscard]] const data_type &at(const grid_size<dimensions> &pos) const
		{
			return m_data[m_dim.index_of(pos)];
		}


		// iterators

	#pragma endregion
	#pragma region partitons (subgrid, slice)

		// subgrid (single layer), slice (vector of layers)

	#pragma endregion

		// kernel shennanigans?

	#pragma region member variables

	private:
		grid_size<dimensions> m_dim;
		std::vector<data_type> m_data;

	#pragma endregion
	};

#pragma endregion
}