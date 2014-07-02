#include "FF4NodeSource.h"
#include "FF4Utils.h"

ff4psp::impl::NodeSource::NodeSource(const std::string& name, long off, long s)  : filename(name), offset(off), size(s)
{
	boost::filesystem::path p = ff4psp::utils::encodings::path(filename);
	stream.open(p,std::ios::in|std::ios::binary);
	stream.seekg(offset);												
}