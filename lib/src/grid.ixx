export module p3.grid;
/*
	Grid module, part of github/TeraFlint/pitrilib.
	Daniel Wiegert (Pitri), 2021.
*/

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
// the standard says it should be, yet it hasn't been implemented, yet?
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

		/*
			returns the number of elements for the whole grid (axis = 0) or any "layer" inside the grid (axis > 0)
		*/
		[[nodiscard]] constexpr size_t elements(size_t most_significant_axis = 0) const
		{
			auto rend = this->rbegin();
			std::advance(rend, dimensions - most_significant_axis);

			// todo: import <numeric>; import <functional>;
			// return std::accumulate(this->rbegin(), rend, 1, std::multiplies{});
			size_t result = 1;
			for (auto iter = this->rbegin(); iter != rend; ++iter)
			{
				result *= (*iter);
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

		[[nodiscard]] constexpr grid_size<dimensions - 1> remove_axis(size_t axis) const
			requires(dimensions > 0)
		{
			grid_size<dimensions - 1> result;
			auto source = this->begin();
			auto destination = result.begin();

			const auto copy_n = [&](size_t n) 
			{
				for (size_t i = 0; i < n; ++i, ++destination, ++source)
				{
					*destination = *source;
				}
			};

			copy_n(axis);
			++source;
			copy_n(dimensions - axis - 1);
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
#pragma region grid generators + concept

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

	namespace grid_gen
	{
		export
		template <size_t dimensions>
		constexpr size_t ascending(const p3::grid_pos<dimensions> &pos)
		{
			return pos.index();
		}

		export
		template <typename data_type>
		constexpr auto positional_notation(size_t base = 10)
		{
			return [=](const auto &pos)
			{
				data_type index = 0, layer_size = 1;
				for (auto iter = pos.pos().rbegin(); iter != pos.pos().rend(); ++iter)
				{
					index += (*iter) * layer_size;
					layer_size *= base;
				}
				return index;
			};
		}

		export
		template <typename data_type, typename function_type>
		constexpr auto axis_binary_operation(const data_type &identity = {}, const function_type &binary_operation = {})
		{
			return [=](const auto &pos)
			{
				auto result = identity;
				for (const auto &val : pos.pos())
				{
					result = binary_operation(result, val);
				}
				return result;
			};
		}

		export
		template <typename data_type>
		constexpr auto axis_add()
		{
			return axis_binary_operation<data_type, std::plus<data_type>>();
		}

		export
		template <typename data_type>
		constexpr auto axis_mul()
		{
			return axis_binary_operation<data_type, std::multiplies<data_type>>(1);
		}

		export
		template <typename data_type>
		constexpr auto axis_xor()
		{
			// todo: import <functional>;
			// return axis_binary_operation<data_type, std::bit_xor<data_type>>();

			return axis_binary_operation<data_type>(0, [](const auto &a, const auto &b) { return a ^ b; });
		}
	}

#pragma endregion
#pragma region grid

	export
	/*
		n dimensional grid class. can represent a vector, matrix, 3d voxel structure, etc.
	*/
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
		explicit VEC_CXP grid(const grid_size<dimensions> &size, generator_type generator)
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
		explicit VEC_CXP grid(const grid<compatible_type, dimensions> &other, converter_type converter)
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

		[[nodiscard]] constexpr bool inside(const grid_size<dimensions> &pos) const
		{
			const auto check = [](auto dim, auto pos) { return pos < dim; };
			return dual_iteration_n(dimensions, m_dim.begin(), pos.begin(), check);
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

		// read-write iteration (with position information)
		template <typename function_type>
		VEC_CXP void iterate(const function_type &function)
		{
			grid_pos<dimensions> pos{ m_dim };
			for (auto &item : m_data)
			{
				function(pos, item);
				++pos;
			}
		}

		// read-only iteration (with position information)
		template <typename function_type>
		VEC_CXP void iterate(const function_type &function) const
		{
			grid_pos<dimensions> pos{ m_dim };
			for (const auto &item : m_data)
			{
				function(pos, item);
				++pos;
			}
		}

		// preserves the positions of elements in the grid. cut-off elements due to axis shrinkage will be lost.
		VEC_CXP void resize(const grid_size<dimensions> &size)
		{
			// todo: make it faster (and in-place)
			// it's nice how my constructors and abstractions theoretically allows me to make a oneliner out of this.
			// however, this is a very naive and potentially expensive approach, as a whole grid gets copied. :/
			// this is going to be similarly complicated and brain wrecking as grid::subgrid(), I fear...
			*this = this_type(size, [&](const auto &pos) { return this->inside(pos.pos()) ? this->at(pos.pos()) : data_type{}; });
		}

	#pragma endregion
	#pragma region partitons (subgrid, slice)

	private:
		struct slice_info
		{
			slice_info() = delete;

			explicit constexpr slice_info(const grid_size<dimensions> &original_size, size_t axis)
				requires(dimensions > 0)
				: size{ original_size.remove_axis(axis) }, period{ size.elements(axis) }, delta{ original_size.elements(axis) }
			{
			}

			constexpr size_t calc_offset(size_t layer)
			{
				return layer * period;
			}
			constexpr size_t calc_jump() const
			{
				return delta - period;
			}

			const grid_size<dimensions - 1> size;
			const size_t period, delta;
		};
		struct subgrid_info : slice_info
		{
			subgrid_info() = delete;

			explicit constexpr subgrid_info(const grid_size<dimensions> &original_size, size_t layer, size_t axis)
				requires(dimensions > 0)
				: slice_info(original_size, axis), offset{ this->calc_offset(layer) }, jump{ this->calc_jump() }
			{
			}

			explicit constexpr subgrid_info(const slice_info &info, size_t layer)
				requires(dimensions > 0)
				: slice_info{ info }, offset{ this->calc_offset(layer) }, jump{ this->calc_jump() }
			{
			}

			const size_t offset, jump;
		};

	public:
		/*
			cuts one slice/layer out of the grid.
			the axis is perpendicular to the cut (the slice contains all the values with dim_at(axis) == layer).
		*/
		VEC_CXP grid<data_type, dimensions - 1> subgrid(size_t layer, size_t axis = 0) const
			requires(dimensions > 0)
		{
			const subgrid_info info(m_dim, layer, axis);
			return grid<data_type, dimensions - 1>(info.size, subgrid_generator(info));
		}

		/*
			slices the whole grid into dim_at(axis) layers.
			the axis is perpendicular to the cut (the slice contains all the values with dim_at(axis) == layer).
		*/
		VEC_CXP std::vector<grid<data_type, dimensions - 1>> slice(size_t axis = 0) const
			requires(dimensions > 0)
		{
			std::vector<grid<data_type, dimensions - 1U>> result;
			result.reserve(m_dim[axis]);

			const slice_info general_info(m_dim, axis);
			for (size_t layer = 0; layer < m_dim[axis]; ++layer)
			{
				const subgrid_info layer_info(general_info, layer);
				result.emplace_back(layer_info.size, subgrid_generator(layer_info));
			}
			return result;
		}

	private:
		constexpr auto subgrid_generator(const subgrid_info &info) const
			requires(dimensions > 0)
		{
			return [info, ptr = &m_data[info.offset], progress = 0](const auto &pos) mutable
				-> data_type
			{
				const data_type result = *ptr;
				++ptr;
				if (++progress >= info.period)
				{
					progress = 0;
					ptr += info.jump;
				}
				return result;
			};
		}

	#pragma endregion
	#pragma region member variables

	private:
		grid_size<dimensions> m_dim{};
		container_type m_data;

	#pragma endregion
	};

#pragma endregion
}