export module p3.grid;
import <vector>;

namespace p3
{
	export
	template <typename data_type, size_t dim>
	class grid
	{
	public:

		// various constructors
		// accessors, iterators
		// subgrid (single layer), slice (vector of layers)

	private:
		std::vector<data_type> m_data;
		// size
	};

}