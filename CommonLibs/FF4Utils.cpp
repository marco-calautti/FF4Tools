#include "FF4Utils.h"

#include <boost/filesystem/detail/utf8_codecvt_facet.hpp> 
#include <boost/locale.hpp>

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

std::string ff4psp::utils::encodings::to_utf8(const std::wstring& original)
{
	return boost::locale::conv::utf_to_utf<char>(original);
}