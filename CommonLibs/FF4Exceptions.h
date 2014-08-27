#pragma once

#include <stdexcept>
#include <string>
#include <boost/format.hpp>

namespace ff4psp
{
	class FF4Exception : public std::runtime_error
	{
	public:
		FF4Exception(const std::string& err) : std::runtime_error(err) {}

		FF4Exception(const char* err) : std::runtime_error(err) {}

		static const FF4Exception raise(const std::string& err)
		{
			throw FF4Exception(err);
		}

		static const FF4Exception raise(boost::format& fmt)
		{
			throw FF4Exception(fmt.str());
		}
	};
}