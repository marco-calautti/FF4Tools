#pragma once

#include <string>
#include <boost/filesystem/path.hpp>

namespace ff4psp
{
	namespace utils
	{
		namespace encodings
		{
			boost::filesystem::path path(const std::string& str);
			boost::filesystem::path path(const char* str);

			boost::filesystem::path path_native(const std::string& str);
			boost::filesystem::path path_native(const char* str);

			std::string to_utf8(const std::wstring& original);

			std::string path_string(const boost::filesystem::path& path);
		}
	}
	
}