#include "FF4Utils.h"

#include <boost/filesystem/detail/utf8_codecvt_facet.hpp> 
#include <boost/locale.hpp>
#include <boost/predef.h>

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

boost::filesystem::path ff4psp::utils::encodings::path_native(const std::string& str)
{
	return path(boost::locale::conv::to_utf<char>(str, std::locale()));
}

boost::filesystem::path ff4psp::utils::encodings::path_native(const char* str)
{
	return path(boost::locale::conv::to_utf<char>(str, std::locale()));
}

std::string ff4psp::utils::encodings::to_utf8(const std::wstring& original)
{
	return boost::locale::conv::utf_to_utf<char>(original);
}

std::string ff4psp::utils::encodings::path_string(const boost::filesystem::path& path)
{

#ifdef BOOST_OS_WINDOWS

	return to_utf8(path.wstring());

#else

	return path.string();

#endif
}