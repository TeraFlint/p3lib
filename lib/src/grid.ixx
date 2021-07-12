export module p3.grid;
import <vector>;
import <array>;

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
	struct grid_size : std::array<size_t, dim>
	{
		[[nodiscard]] constexpr size_t elements() const
		{
			// return std::accumulate(this->begin(), this->end(), 1, std::multiplies{});

			// import <numeric>; import <functional>; => "there are too many errors for the intellisense engine to function properly"
			// until that's fixed, I'll keep a manual implementation, instead. At least as long as I'm stil developing this class.
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

			auto size_it = this->rbegin();
			auto pos_it = position.rbegin();

			for (size_t i = 0; i < dim; ++i, ++pos_it, ++size_it)
			{
				result += layer_size * (*pos_it);
				layer_size *= (*size_it);
			}
			return result;
		}
		[[nodiscard]] constexpr size_t index_in(const grid_size<dim> &boundary) const
		{
			return boundary.index_of(*this);
		}
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

		// nested list constructor

		// size + list constructor
		explicit grid(const grid_size<dim> &size, const std::initializer_list<data_type> &init = {})
			: m_size(size), m_data(m_size.elements())
		{

		}

		// size + generator constructor

		// grid + converter constructor



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
			return m_size.elements();
		}

#pragma endregion
#pragma region accessors and iterators

		// accessors, iterators

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