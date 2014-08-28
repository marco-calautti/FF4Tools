#include "FF4NodeSource.h"
#include "FF4Utils.h"

ff4psp::NodeSource::NodeSource(const std::string& name, long off, long s, SourceType t) : filename(name), 
																						  offset(off), 
																						  size(s), 
																						  stream(ff4psp::utils::encodings::path(filename), std::ios::in | std::ios::binary)
{
	type = t;
	stream.seekg(offset);	

}

size_t ff4psp::NodeSource::read(char* buffer, size_t count)
{
	stream.read(buffer, count);
	return stream.gcount();
}