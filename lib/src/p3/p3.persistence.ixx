#include <filesystem>
#include <iostream>
#include <fstream>

export module p3.persistence;

// import <filesystem>;
// import <iostream>;
// import <fstream>;

namespace p3
{
#pragma region helpers

	class stream_flag_saver
	{
	public:
		stream_flag_saver(std::ios &stream)
			: m_stream(stream), m_flags(stream.flags())
		{
		}

		~stream_flag_saver()
		{
			m_stream.flags(m_flags);
		}

	private:
		std::ios &m_stream;
		std::ios_base::fmtflags m_flags;
	};

#pragma endregion
#pragma region file_access

	export
	class file_access
	{
	public:
		using path_type = std::filesystem::path;

		class no_file      : public std::exception { using std::exception::exception; };
		class no_directory : public std::exception { using std::exception::exception; };
		class custom_error : public std::exception { using std::exception::exception; };

		void load(const path_type &location)
		{
			if (!std::filesystem::exists(location))
			{
				throw no_file("loading: file not found.");
			}

			if (!on_load(location))
			{
				throw custom_error("loading: on_load returned false.");
			}
		}

		void save(const path_type &location, bool create = false) const
		{
			if (!directory_exists(location))
			{
				if (!create || !create_path(location))
				{
					throw no_directory("saving: directory not found.");
				}
			}

			if (!on_save(location))
			{
				throw custom_error("saving: on_safe returned false.");
			}
		}

		static bool directory_exists(const path_type &location)
		{
			namespace fs = std::filesystem;
			if (fs::is_directory(location))
			{
				return true;
			}
			const fs::path parent = location.parent_path();
			return parent.empty() ? true : fs::exists(parent);
		}

		static bool create_path(const path_type &location)
		{
			namespace fs = std::filesystem;
			return fs::is_directory(location) ?
				fs::create_directories(location) :
				fs::create_directories(location.parent_path());
		}

	protected:
		virtual bool on_load(const path_type &location) = 0;
		virtual bool on_save(const path_type &location) const = 0;
	};

#pragma endregion
#pragma region stream_access

	export
	class stream_access
	{
	public:
		class custom_error : public std::exception { using std::exception::exception; };

		void read(std::istream &stream, bool reset_if_fail = false)
		{
			stream_flag_saver flags(stream);
			stream.tellg();

			if (!on_read(stream))
			{
				if (reset_if_fail)
				{
					stream.seekg(0);
				}
				throw custom_error("reading: on_read returned false.");
			}
		}
		void write(std::ostream &stream) const
		{
			stream_flag_saver flags(stream);

			if (!on_write(stream))
			{
				throw custom_error("writing: on_write returned false.");
			}
		}

	protected:
		virtual bool on_read (std::istream &stream) = 0;
		virtual bool on_write(std::ostream &stream) const = 0;
	};

#pragma endregion
#pragma region filestream_access

	export
	class filestream_access
		: public file_access, public stream_access
	{
	protected:
		bool on_load(const path_type &location) override
		{
			std::ifstream file(location);
			read(file);
			return true;
		}
		bool on_save(const path_type &location) const override
		{
			std::ofstream file(location);
			write(file);
			return true;
		}
	};

#pragma endregion
}
