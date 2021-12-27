export module p3.vector.dynamic;

export import p3.vector.dense;
export import p3.vector.sparse;

// export import p3.adapting_container;

namespace p3
{

	// template <typename data_type>
	// using vector_storage_behavior = adapting_container
	// <
	// 	dense_vector <data_type>,
	// 	sparse_vector<data_type>
	// >;
	
	export
	template <typename data_type>
	class dynamic_vector // : vector_storage_behavior<data_type>
	{
		// todo: 
	};
}