#include "FF4Utils.h"

#include <boost/filesystem/detail/utf8_codecvt_facet.hpp> 

static boost::filesystem::detail::utf8_codecvt_facet utf8;

boost::filesystem::path ff4psp::utils::encodings::path(const std::string& str)
{
	boost::filesystem::path p;
	p.assign(str,utf8);
	return p;
}

boost::filesystem::path ff4psp::utils::encodings::path(const char* str)
{
	boost::filesystem::path p;
	p.assign(str,utf8);
	return p;
}