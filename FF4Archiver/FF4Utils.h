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
		}
	}
	
}