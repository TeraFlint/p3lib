export module p3.grid;
import <functional>; // std::multiplies
import <numeric>;    // std::accumulate
import <vector>;
import <array>;      // grid_size

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
			return std::accumulate(this->begin(), this->end(), 1, std::multiplies{});
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