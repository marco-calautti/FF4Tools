#include "FF4FileFilters.h"
#include "FF4Exceptions.h"
#include "FF4Utils.h"

#include "Lzss.h"

#include <boost/filesystem/fstream.hpp>

const int BUFFER_SIZE=0x800;

void ff4psp::impl::NormalFilter::operator()(NodeSource* source, const std::string& path)
{

	boost::filesystem::path p=ff4psp::utils::encodings::path(path);
	boost::filesystem::ofstream output(p, std::ios::out|std::ios::binary);

	std::istream& file=source->getStream();


	if(!file)
		FF4Exception::raise("Could not open the archive file. Check if it does exist and it is accessible.");

	if(!output)
		FF4Exception::raise("Could not create output file. Check if it is accessible");

	char buffer[BUFFER_SIZE];

	int sectors=source->getSize()/BUFFER_SIZE;

	for(int i=0;i<sectors;i++)
	{
		file.read(buffer,BUFFER_SIZE);
		if(!file)
			FF4Exception::raise("Error while reading from archive. Check that it exists and it is accessible.");

		output.write(buffer,BUFFER_SIZE);

		if(!output)
			FF4Exception::raise("Error while writing the file. Check that it is accessible.");
	}

	int remainingBytes=source->getSize()-sectors*BUFFER_SIZE;
	if(remainingBytes==0) return;

	file.read(buffer,remainingBytes);
	if(!file)
		FF4Exception::raise("Error while reading from archive. Check that it exists and it is accessible.");

	output.write(buffer,remainingBytes);

	if(!output)
		FF4Exception::raise("Error while writing the file. Check that it is accessible.");
}

void ff4psp::impl::DecompressFilter::operator()(NodeSource* source, const std::string& path)
{
	NormalFilter normalFilter;

	boost::filesystem::path p=ff4psp::utils::encodings::path(path);

	if(p.extension().string() != ".lzs")
	{
		normalFilter(source,path);
		return;
	}

	boost::filesystem::ofstream output(p, std::ios::out|std::ios::binary);

	std::istream& file=source->getStream();


	if(!file)
		FF4Exception::raise("Could not open the archive file. Check if it does exist and it is accessible.");

	if(!output)
		FF4Exception::raise("Could not create output file. Check if it is accessible");

	uint8_t* buffer = new uint8_t[source->getSize()];
	uint8_t* dest;

	file.read((char*)buffer,source->getSize());

	int decSize=ff4psp::compression::LzssDecode(dest,buffer,source->getSize());

	output.write((char*)dest,decSize);

	delete[] buffer;
	delete[] dest;

	if(!output)
		FF4Exception::raise("Error while writing the file. Check that it is accessible.");
}