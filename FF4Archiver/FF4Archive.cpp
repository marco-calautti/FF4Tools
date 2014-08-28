#include <sstream>
#include <iomanip>
#include <memory>

#include <boost/shared_array.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "FF4ArchiveNodeImpl.h"
#include "FF4Archive.h"
#include "FF4Internals.h"
#include "FF4Exceptions.h"
#include "FF4Utils.h"
#include "FF4FileFilters.h"
#include "FF4ArchiveVisitors.h"


const std::string ff4psp::FF4Archive::DEFAULT_INDEX_FILENAME="PAC0.bin";
const std::string ff4psp::FF4Archive::DEFAULT_DATA_FILENAME="PAC1.bin";

ff4psp::FF4Archive::FF4Archive(const std::string& PAC0, const std::string& PAC1) : 
							   indexFile(PAC0), dataFile(PAC1), root(new impl::ArchiveNodeImpl())
{
	boost::filesystem::ifstream index(ff4psp::utils::encodings::path(indexFile),std::ios::in|std::ios::binary);

	if(!index)
		FF4Exception::raise("Could not open index file");

	ff4psp::impl::Header header;
	index.read((char*)&header,sizeof(ff4psp::impl::Header));

	boost::shared_array<ff4psp::impl::Record> records(new ff4psp::impl::Record[header.records]);
	index.read((char*)records.get(),sizeof(ff4psp::impl::Record)*header.records);

	boost::shared_array<ff4psp::impl::FileInfo> files(new ff4psp::impl::FileInfo[header.number_of_files]);
	index.read((char*)files.get(),sizeof(ff4psp::impl::FileInfo)*header.number_of_files);

	boost::shared_array<char> names_table(new char[header.names_table_size]);
	index.read((char*)names_table.get(),header.names_table_size);

	index.close();
		
	if(index.fail() || index.bad() )
		FF4Exception::raise("Error while reading from index file!");

	int i=0;
	constructRootNode(root.get(),records,i,files,names_table);
}

void ff4psp::FF4Archive::constructRootNode(ff4psp::impl::ArchiveNodeImpl* node, 
										   const boost::shared_array<ff4psp::impl::Record>& records, 
										   int& rIndex, 
										   const boost::shared_array<ff4psp::impl::FileInfo>& files, 
										   const boost::shared_array<char>& names_table)
{
	ff4psp::impl::Record r=records[rIndex];

	for(uint32_t i=0;i<r.number_of_fileinfos;i++)
	{
		ff4psp::impl::FileInfo curFile=files[r.fileinfos_offset+i];
		char* fileName=&names_table[curFile.filename_offset];

		std::string name(fileName);
		
		std::unique_ptr<ff4psp::impl::ArchiveNodeImpl> child(new impl::ArchiveNodeImpl(curFile,name,dataFile,node));
		
		ff4psp::impl::ArchiveNodeImpl* temp=child.get();

		node->addChild(std::move(child));

		if(!temp->isFile())
		{
			rIndex++;
			constructRootNode(temp,records,rIndex,files,names_table);
		}
	}
}

ff4psp::ArchiveNode* ff4psp::FF4Archive::getNodeFromPath(const std::string& path) const
{
	impl::NodeRetrieveVisitor visitor;
	return visitor.visit(root.get(), path);
}

void ff4psp::FF4Archive::extract(const ArchiveNode* path, 
								 const std::string& outputDirectory) const
{
	impl::ExtractionVisitor<impl::DecompressFilter> visitor;
	visitor.visit(const_cast<ArchiveNode*>(path), outputDirectory);
}

void ff4psp::FF4Archive::import(ArchiveNode* path,
								const std::string& inputDirectory)
{
	impl::ImportVisitor visitor;
	visitor.visit(path, inputDirectory);

}

void ff4psp::FF4Archive::build(const std::string& iPath, const std::string& aPath) const
{
	boost::filesystem::path indexPath(iPath);
	

	boost::filesystem::path archivePath(aPath);

	boost::filesystem::ofstream indexStream(indexPath, std::ios::out | std::ios::binary);
	boost::filesystem::ofstream archiveStream(archivePath, std::ios::out | std::ios::binary);

	impl::BuildVisitor<impl::CompressFilter> visitor(indexStream);

	visitor.visit(root.get(),archiveStream);
}