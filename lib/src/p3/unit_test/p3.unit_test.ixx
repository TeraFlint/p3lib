export module p3.unit_test;

export import p3.unit_test.core;
export import p3.unit_test.assert;

/*
	unit test usage:

	class my_test : public p3::unit_test::test_case<my_test>
	{
		void run_test() const
		{
			// testing here
		}
	};
*/