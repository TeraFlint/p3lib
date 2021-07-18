export module p3.persistence;

// see p3.grid
#define INTELLISENSE_HACK

#if !defined(INTELLISENSE_HACK)
import <filesystem>;
#endif

namespace p3
{
	// here's my plan for 3 similar abstract classes:

	// file_access
	// 	   load(path) -> on_load(path);
	// 	   save(path) -> on_save(path);

	// stream_access
	// 	   read (istream) -> on_read (istream);
	// 	   write(ostream) -> on_write(ostream);

	// filestream_access: public file_access, public stream_access
	// 	   load(path) -> on_load(path) -> read (istream) -> on_read (istream);
	// 	   save(path) -> on_save(path) -> write(ostream) -> on_write(ostream);

#pragma region file_access

	export
	class file_access
	{
	public:
#if defined(INTELLISENSE_HACK)
		using path_type = const char *;
#else
		using path_type = std::filesystem::path;

		class no_file      : public std::exception { using std::exception::exception; };
		class no_directory : public std::exception { using std::exception::exception; };
		class custom_error : public std::exception { using std::exception::exception; };
#endif

		void load(const path_type &location)
		{
#if !defined(INTELLISENSE_HACK)
			if (!std::filesystem::exists(location))
			{
				throw no_file("loading: file not found.");
			}
#endif
			if (!on_load(location))
			{
#if defined(INTELLISENSE_HACK)
				throw "loading: on_load returned false.";
#else
				throw custom_error("loading: on_load returned false.");
#endif
			}
		}

		void save(const path_type &location, bool create = false) const
		{
#if !defined(INTELLISENSE_HACK)
			if (!directory_exists(location))
			{
				if (!create || !create_path(location))
				{
					throw no_directory("saving: directory not found.");
				}
			}
#endif
			if (!on_save(location))
			{
#if defined(INTELLISENSE_HACK)
				throw "saving: on_safe returned false.";
#else
				throw custom_error("saving: on_safe returned false.");
#endif
			}
		}

		static bool directory_exists(const path_type &location)
		{
#if defined(INTELLISENSE_HACK)
			return true;
#else
			namespace fs = std::filesystem;
			if (fs::is_directory(location))
			{
				return true;
			}
			const fs::path parent = location.parent_path();
			return parent.empty() ? true : fs::exists(parent);
#endif
		}

		static bool create_path(const path_type &location)
		{
#if defined(INTELLISENSE_HACK)
			return true;
#else
			namespace fs = std::filesystem;
			return fs::is_directory(location) ?
				fs::create_directories(location) :
				fs::create_directories(location.parent_path());
#endif
		}

	protected:
		virtual bool on_load(const path_type &location) = 0;
		virtual bool on_save(const path_type &location) const = 0;
	};

#pragma endregion

}
