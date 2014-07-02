#include "ff4ArchiveNodeImpl.h"

#include <sstream>
#include <iomanip>

ff4psp::impl::ArchiveNodeImpl::ArchiveNodeImpl()
{
	setParent(nullptr);
	setIsFile(false);
	setName("root");
	setFileSize(0);
	setChecksum("");
}

ff4psp::impl::ArchiveNodeImpl::ArchiveNodeImpl(const ff4psp::impl::FileInfo& info, const std::string& name, const std::string& srcName,
				ArchiveNode* parent) : sourceName(srcName), offset(info.file_offset)
{
	setParent(parent);
	setName(name);
	setFileSize(info.file_real_size);
	setIsFile(info.is_file==1);

	std::stringstream ss;
	ss.fill('0');
	for(int c=0;c<ff4psp::impl::CHECKSUM_SIZE;c++)
		ss << std::setw(2) << std::hex << static_cast<uint32_t>(info.sha_256[c]);

	setChecksum(ss.str());
}