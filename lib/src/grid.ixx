export module p3.grid;
import <vector>;
import <array>;

// import <functional>;
// import <stdexcept>;
// import <algorithm>;
// import <numeric>;

// here's a problem: intellisense is not happy with these imports. If any of them is imported, I get:
// "E2996: There are too many errors for the IntelliSense engine to function properly".
// the problem is: I'd rather have intellisense features during development than getting shorter code.
// so I will provide the oneliners, but add a todo comment and do the manual implementation. That way I still get support from the IDE.
// once the classes are finished, I add the imports it and swap out the manual implementations.
// thanks to the added unit tests, I'll then be able to see if it broke or not.


// also, big todo: replace this with actual constexpr once std::vector is actually constexpr compatible...
#define VEC_CXP

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
			: m_dim{ size }, m_pos{}
		{
		}

		constexpr explicit grid_pos(const grid_size<dimensions> &size, const grid_size<dimensions> &position)
			: m_dim{ size }, m_pos{ position }
		{
			const auto check = [](auto dim, auto pos) { return pos < dim; };
			const bool in_bounds = dual_iteration_n(dimensions, m_dim.begin(), m_pos.begin(), check);

			if (!in_bounds)
			{
				// todo: import <stdexcept>;
				// throw std::out_of_range("grid_pos: position is outside its boundaries.");
				throw "grid_pos: position is outside its boundaries.";
			}
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
		[[nodiscard]] constexpr size_t index() const
		{
			return m_dim.index_of(m_pos);
		}

		[[nodiscard]] VEC_CXP auto operator<=>(const grid_pos<dimensions> &other) const = default;

	#pragma endregion
	#pragma region manipulators

	public:
		constexpr void first()
		{
			m_pos = {};
			m_valid = true;
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

			m_valid = true;
		}

		constexpr bool jump(const grid_size<dimensions> &pos)
		{
			const auto check = [](auto dim, auto pos) { return pos < dim; };
			const bool in_bounds = dual_iteration_n(dimensions, m_dim.begin(), pos.begin(), check);
			if (in_bounds)
			{
				m_pos = pos;
				m_valid = true;
			}
			return in_bounds;
		}

		constexpr bool next(size_t axis = 0)
		{
			const auto can_terminate = [](const auto &dim, auto &pos) { return pos + 1 < dim; };
			const auto operation     = [](const auto &dim, auto &pos) { ++pos; };
			const auto reset         = [](const auto &dim, auto &pos) { pos = 0; };
			return iterate_backwards_until(can_terminate, operation, reset, axis);
		}

		constexpr bool prev(size_t axis = 0)
		{
			const auto can_terminate = [](const auto &dim, auto &pos) { return pos > 0; };
			const auto operation     = [](const auto &dim, auto &pos) { --pos; };
			const auto reset         = [](const auto &dim, auto &pos) { pos = dim - 1; };
			return iterate_backwards_until(can_terminate, operation, reset, axis);
		}

	private:
		template <typename check_type, typename operation_type, typename reset_type>
		constexpr bool iterate_backwards_until(const check_type &can_terminate, const operation_type &operation, const reset_type &reset, const size_t axis)
		{
			auto dim_iter = m_dim.crbegin();
			auto pos_iter = m_pos.rbegin();

			std::advance(dim_iter, axis);
			std::advance(pos_iter, axis);

			for (size_t i = 0; i < dimensions - axis; ++i, ++dim_iter, ++pos_iter)
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
		grid_size<dimensions> m_dim, m_pos;
		bool m_valid = true;

	#pragma endregion
	};

#pragma endregion
#pragma region grid modifier concept

	template <typename data_type>
	concept any_type = true;

	// This concept is here to disambiguate between initializer_list and a generator function.
	// It makes sure it receives a function-like type capable of accepting a grid_pos.
	// The return type doesn't matter as all the algorithms using it discard it.
	// So void functions would make sense, but I'd like to stay compatible to other functions, as well.
	export
	template <typename function_type, size_t dim>
	concept grid_generator = requires(function_type function)
	{
		{ function(grid_pos<dim>{}) } -> any_type;
	};

#pragma endregion
#pragma region grid

	export
	template <typename data_type, size_t dimensions>
	class grid
	{
	#pragma region types

public:
	using this_type = grid<data_type, dimensions>;
	using container_type = std::vector<data_type>;
	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using reverse_iterator = typename container_type::reverse_iterator;
	using const_reverse_iterator = typename container_type::const_reverse_iterator;

	#pragma endregion
	#pragma region constructors

	public:
		// default constructor
		VEC_CXP grid() = default;

		// implicit nested list constructor
		// grid(const nested_list_t<data_type, dimensions> &list);

		// size constructor
		explicit VEC_CXP grid(const grid_size<dimensions> &size)
			: m_dim{ size }, m_data(m_dim.elements())
		{
		}

		// size + list constructor
		explicit VEC_CXP grid(const grid_size<dimensions> &size, const std::initializer_list<data_type> &init)
			: m_dim{ size.fit_to_data(init.size()) }
		{
			const auto fill = [&]()
			{
				// todo: import <algorithm>;
				// std::copy(init.begin(), init.end(), std::back_inserter(m_data));
				for (const auto &item : init)
				{
					m_data.push_back(item);
				}
			};
			reserve_fill_resize(fill);
		}

		// size + iterator pair constructor
		template <typename begin_type, typename end_type>
		explicit VEC_CXP grid(const grid_size<dimensions> &size, const begin_type &begin, const end_type &end)
			: m_dim{ size.fit_to_data(std::distance(begin, end)) }
		{
			const auto fill = [&]() 
			{
				// todo: import <algorithm>;
				// std::copy(begin, end, std::back_inserter(m_data));
				for (auto iter = begin; iter != end; ++iter)
				{
					m_data.push_back(*iter);
				}
			};
			reserve_fill_resize(fill);
		}

		// size + generator constructor
		template <grid_generator<dimensions> generator_type>
		explicit VEC_CXP grid(const grid_size<dimensions> &size, const generator_type &generator)
			: m_dim{ size }
		{
			const auto fill = [&]()
			{
				for (grid_pos<dimensions> pos{ m_dim }; pos.valid(); ++pos)
				{
					m_data.push_back(generator(pos));
				}
			};
			reserve_fill_resize(fill);
		}

		// grid + converter constructor
		template <typename compatible_type, typename converter_type>
		explicit VEC_CXP grid(const grid<compatible_type, dimensions> &other, const converter_type &converter)
			: m_dim{ other.dim() }
		{
			const auto fill = [&]()
			{
				grid_pos<dimensions> pos{ m_dim };
				for (const auto &val : other)
				{
					m_data.push_back(converter(pos, val));
					++pos;
				}
			};
			reserve_fill_resize(fill);
		}

		// implicit conversion constructor
		template <typename compatible_type>
		VEC_CXP grid(const grid<compatible_type, dimensions> &other) 
			requires(std::is_convertible_v<compatible_type, data_type>)
			: grid(other, [](auto pos, auto val) { return val; })
		{
		}

	private:
		template <typename function_type>
		VEC_CXP void reserve_fill_resize(const function_type &fill)
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
		[[nodiscard]] VEC_CXP auto operator<=>(const this_type &other) const = default;

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

		[[nodiscard]] constexpr size_t index_of(const grid_size<dimensions> &pos) const
		{
			return m_dim.index_of(pos);
		}

		[[nodiscard]] constexpr grid_size<dimensions> position_of(size_t index) const
		{
			return grid_size<dimensions>::from_index(index, m_dim);
		}

	#pragma endregion
	#pragma region accessors

	public:
		[[nodiscard]] VEC_CXP data_type &operator[](size_t index)
		{
			return m_data[index];
		}

		[[nodiscard]] VEC_CXP const data_type &operator[](size_t index) const
		{
			return m_data[index];
		}

		[[nodiscard]] VEC_CXP data_type &at(const grid_size<dimensions> &pos)
		{
			return m_data[m_dim.index_of(pos)];
		}

		[[nodiscard]] VEC_CXP const data_type &at(const grid_size<dimensions> &pos) const
		{
			return m_data[m_dim.index_of(pos)];
		}

	#pragma endregion
	#pragma region iterators

	public:
		[[nodiscard]] iterator begin()
		{
			return m_data.begin();
		}
		[[nodiscard]] const_iterator begin() const
		{
			return m_data.begin();
		}
		[[nodiscard]] iterator end()
		{
			return m_data.end();
		}
		[[nodiscard]] const_iterator end() const
		{
			return m_data.end();
		}

		[[nodiscard]] reverse_iterator rbegin()
		{
			return m_data.rbegin();
		}
		[[nodiscard]] const_reverse_iterator rbegin() const
		{
			return m_data.rbegin();
		}
		[[nodiscard]] reverse_iterator rend()
		{
			return m_data.rend();
		}
		[[nodiscard]] const_reverse_iterator rend() const
		{
			return m_data.rend();
		}

		[[nodiscard]] const_iterator cbegin() const
		{
			return m_data.cbegin();
		}
		[[nodiscard]] const_iterator cend() const
		{
			return m_data.cend();
		}

		[[nodiscard]] const_iterator crbegin() const
		{
			return m_data.crbegin();
		}
		[[nodiscard]] const_iterator crend() const
		{
			return m_data.crend();
		}

	#pragma endregion
	#pragma region manipulators

	public:
		// preserves the positions of elements in the grid. cut-off elements due to axis shrinkage will be lost.

		// void resize(const grid_size<dimensions> &size);

	#pragma endregion
	#pragma region partitons (subgrid, slice)

	public:
		// in both cases we may choose the axis perpendicular to the cut (axis of constant coordinates)

		VEC_CXP grid<data_type, dimensions - 1U> subgrid(size_t layer, size_t axis = 0) const
		{
			// todo
			return {};
		}

		VEC_CXP std::vector<grid<data_type, dimensions - 1U>> slice(size_t axis = 0) const
		{
			// todo: write subgrid and find a good abstraction which we can use to either fill one subgrid or a whole sliced grid simultaneously and efficiently.
			std::vector<grid<data_type, dimensions - 1U>> result;
			result.reserve(m_dim[axis]);
			for (size_t layer = 0; layer < m_dim[axis]; ++layer)
			{
				result.push_back(subgrid(layer, axis));
			}
			return result;
		}

	private:
		// VEC_EXP void populate_subgrid() const;


	#pragma endregion
	#pragma region member variables

	private:
		grid_size<dimensions> m_dim{};
		container_type m_data;

	#pragma endregion
	};

#pragma endregion
}