export module p3.grid;
import <vector>;
import <array>;

namespace p3
{
#pragma region nested list

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
	};

#pragma endregion
#pragma region grid

	export
	template <typename data_type, size_t dim>
	class grid
	{
	public:

		// various constructors
		// accessors, iterators
		// subgrid (single layer), slice (vector of layers)
		// kernel?

	private:
		std::vector<data_type> m_data;
		// size
	};

#pragma endregion

}