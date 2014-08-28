#include "ff4ArchiveNodeImpl.h"

#include <sstream>
#include <iomanip>

#include "sha2.h"

ff4psp::impl::ArchiveNodeImpl::ArchiveNodeImpl()
{
	setParent(nullptr);
	setIsFile(false);
	setName("root");
}

ff4psp::impl::ArchiveNodeImpl::ArchiveNodeImpl(const ff4psp::impl::FileInfo& info, const std::string& name, const std::string& srcName,
				ArchiveNode* parent)
{
	setParent(parent);
	setName(name);
	setSourceFile(srcName, info.file_offset, info.file_real_size, NodeSource::SourceType::Original);
	setIsFile(info.is_file==1);

	//std::stringstream ss;
	//ss.fill('0');
	//for (int c = 0; c < CHECKSUM_SIZE; c++)
		//ss << std::setw(2) << std::hex << static_cast<uint32_t>(info.sha_256[c]);
	std::memcpy(checksum, info.sha_256, CHECKSUM_SIZE);
	//setChecksum(ss.str());
}

void ff4psp::impl::ArchiveNodeImpl::setSourceFile(const std::string& filename, long off, long size,ff4psp::NodeSource::SourceType type)
{
	if (size >= 0)
		filesize = size;
	else
	{
		filesize = boost::filesystem::file_size(ff4psp::utils::encodings::path(filename));
	}
	offset = off;
	sourceFilename = filename;

	this->type = type;
	if(type==NodeSource::SourceType::File)
		recomputeChecksum = true;
}

std::unique_ptr<ff4psp::NodeSource> ff4psp::impl::ArchiveNodeImpl::createNodeSource() const
{
	return std::unique_ptr<NodeSource>(new NodeSource(sourceFilename, offset, filesize,type));
}

void ff4psp::impl::ArchiveNodeImpl::getChecksum(char chck[CHECKSUM_SIZE]) const
{
	if (recomputeChecksum)
	{
		//compute checksum
		std::unique_ptr<NodeSource> source = createNodeSource();

		char* buffer = new char[source->getSize()];
		source->read(buffer, source->getSize());
		

		SHA256_CTX ctx;
		SHA256_Init(&ctx);

		SHA256_Update(&ctx, (uint8_t*)buffer, source->getSize());
		SHA256_Final((uint8_t*)checksum, &ctx);
		delete[] buffer;
		recomputeChecksum = false;
	}

	std::memcpy(chck, checksum, CHECKSUM_SIZE);
}
