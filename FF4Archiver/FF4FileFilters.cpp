#include "FF4FileFilters.h"
#include "FF4Exceptions.h"
#include "FF4Utils.h"

#include "Lzss.h"
#include "sha2.h"

#include <boost/filesystem/fstream.hpp>

const int BUFFER_SIZE=0x800;

void ff4psp::impl::NormalExtractFilter::operator()(NodeSource* source, const std::string& path)
{

	boost::filesystem::path p=ff4psp::utils::encodings::path(path);
	boost::filesystem::ofstream output(p, std::ios::out|std::ios::binary);

	//std::istream& file=source->getStream();


	//if(!file)
	//	FF4Exception::raise("Could not open the archive file. Check if it does exist and it is accessible.");

	if(!output)
		FF4Exception::raise("Could not create output file. Check if it is accessible");

	char buffer[BUFFER_SIZE];

	int sectors=source->getSize()/BUFFER_SIZE;

	for(int i=0;i<sectors;i++)
	{
		source->read(buffer,BUFFER_SIZE);
		//if(!file)
		//	FF4Exception::raise("Error while reading from archive. Check that it exists and it is accessible.");

		output.write(buffer,BUFFER_SIZE);

		if(!output)
			FF4Exception::raise("Error while writing the file. Check that it is accessible.");
	}

	int remainingBytes=source->getSize()-sectors*BUFFER_SIZE;
	if(remainingBytes==0) return;

	source->read(buffer,remainingBytes);
	//if(!file)
	//	FF4Exception::raise("Error while reading from archive. Check that it exists and it is accessible.");

	output.write(buffer,remainingBytes);

	if(!output)
		FF4Exception::raise("Error while writing the file. Check that it is accessible.");
}

void ff4psp::impl::DecompressFilter::operator()(NodeSource* source, const std::string& path)
{
	NormalExtractFilter normalFilter;

	boost::filesystem::path p=ff4psp::utils::encodings::path(path);

	if (ff4psp::utils::encodings::path_string(p.extension()) != ".lzs")
	{
		normalFilter(source,path);
		return;
	}

	boost::filesystem::ofstream output(p, std::ios::out|std::ios::binary);

	//std::istream& file=source->getStream();


	//if(!file)
	//	FF4Exception::raise("Could not open the archive file. Check if it does exist and it is accessible.");

	if(!output)
		FF4Exception::raise("Could not create output file. Check if it is accessible");

	uint8_t* buffer = new uint8_t[source->getSize()];
	uint8_t* dest;

	source->read((char*)buffer,source->getSize());

	int decSize=ff4psp::compression::LzssDecode(dest,buffer,source->getSize());

	output.write((char*)dest,decSize);

	delete[] buffer;
	delete[] dest;

	if(!output)
		FF4Exception::raise("Error while writing the file. Check that it is accessible.");
}

int ff4psp::impl::NormalBuildFilter::operator()(NodeSource* source, const ArchiveNode* node, char checksum[ArchiveNode::CHECKSUM_SIZE], std::ostream& stream)
{
	long size=source->getSize();

	SHA256_CTX ctx;

	if (source->getType() == NodeSource::SourceType::File)
		SHA256_Init(&ctx);
	else
		node->getChecksum(checksum);

	char* buffer = new char[source->getSize()];
	source->read(buffer, source->getSize());
	if (source->getType() == NodeSource::SourceType::File)
	{
		SHA256_Update(&ctx, (uint8_t*)buffer, source->getSize());
		SHA256_Final((uint8_t*)checksum, &ctx);
	}
	stream.write(buffer, source->getSize());
	delete[] buffer;
	return size;

}

int ff4psp::impl::CompressFilter::operator()(NodeSource* source, const ArchiveNode* node, char checksum[ArchiveNode::CHECKSUM_SIZE], std::ostream& stream)
{
	NormalBuildFilter normalFilter;

	boost::filesystem::path p = ff4psp::utils::encodings::path(node->getName());

	if (ff4psp::utils::encodings::path_string(p.extension()) != ".lzs" || source->getType() == NodeSource::SourceType::Original)
	{
		return normalFilter(source, node, checksum, stream);
	}

	uint8_t* buffer = new uint8_t[source->getSize()];
	uint8_t* dest;

	source->read((char*)buffer, source->getSize());

	int compSize = ff4psp::compression::LzssEncode(dest, buffer, source->getSize());

	SHA256_CTX ctx;
	SHA256_Init(&ctx);
	SHA256_Update(&ctx, (uint8_t*)dest, compSize);
	SHA256_Final((uint8_t*)checksum, &ctx);

	stream.write((char*)dest, compSize);

	delete[] buffer;
	delete[] dest;

	if (!stream)
		FF4Exception::raise("Error while writing the file. Check that it is accessible.");
	return compSize;
}