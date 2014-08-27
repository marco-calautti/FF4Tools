#pragma once

#include <string>

#include <boost/filesystem/fstream.hpp>

#include "FF4NodeSource.h"
#include "FF4ArchiveNodeImpl.h"

namespace ff4psp
{
	class ArchiveNode;

	template<typename S, typename T>
	class ArchiveVisitor
	{
	public:
		virtual ~ArchiveVisitor(){}

		virtual S visit(ArchiveNode* node, T& state)=0;
	};

	namespace impl
	{

		template <typename TFilter>
		class ExtractionVisitor : public ArchiveVisitor<void,const std::string>
		{

		public:
			ExtractionVisitor() { }

			
			virtual void visit(ArchiveNode* node, const std::string& state)
			{
				if(node->isFile())
				{
					boost::filesystem::path filePath = ff4psp::utils::encodings::path(state);
					filePath = filePath / node->getName();
		
					std::unique_ptr<NodeSource> source = node->createNodeSource();

					TFilter filter;
					filter(source.get(),filePath.string());
					return ;
				}

				for(size_t i=0;i<node->size();i++)
				{
					boost::filesystem::path fullPath = ff4psp::utils::encodings::path(state);
					if(!node->getChild(i)->isFile())
					{
						fullPath = fullPath / node->getChild(i)->getName();

						try{ boost::filesystem::create_directory(fullPath); }
						catch(...){ FF4Exception::raise("Could not create directory. Check if the output directory does exist and it is accessible."); }
					}
					std::string path = fullPath.string();
					visit(node->getChild(i),path);
				}
			}
		};


		class NodeRetrieveVisitor : public ArchiveVisitor<ArchiveNode*, const std::string>
		{
		public:
			virtual ArchiveNode* visit(ArchiveNode* node, const std::string& state);

		private:
			bool isValidPath(const std::string& path) const;
		};


		class ImportVisitor : public ArchiveVisitor<void, const std::string>
		{
		public:

			virtual void visit(ArchiveNode* node, const std::string& inputDirectory);

		private:

			void _visit(ArchiveNode* current, const boost::filesystem::path& inputPath);

		};

		template<typename TBuildFilter>
		class BuildVisitor : public ArchiveVisitor<void, std::ostream>
		{
		public:

			BuildVisitor(std::ostream& index) : indexStream(index)
			{
			}

			virtual void visit(ArchiveNode* node, std::ostream& archiveStream)
			{
				records.clear();
				fileInfos.clear();
				names.clear();
				memset(&header, 0, sizeof(header));

				//the archive must contain at least the root record
				header.records = 1;
				header.number_of_files = 0;
				

				int recordCounter = 0, namesIndex=0, filesOffset=0, infosCounter=0;

				_visit(node, archiveStream, -1, filesOffset, recordCounter, infosCounter, -1 ,namesIndex);

				header.number_of_files = infosCounter;
				header.names_table_size = namesIndex;
				header.records = recordCounter;
				header.archive_full_size = sizeof(header)+records.size()*sizeof(Record)+fileInfos.size()*sizeof(FileInfo)+namesIndex;
				header.archive_full_size = header.archive_full_size / 0x800 + (header.archive_full_size % 0x800 == 0 ? 0 : 1);
				header.archive_full_size *= 0x800;
				//write index file
				indexStream.write((char*)&header, sizeof(header));

				for (size_t i = 0; i < records.size(); i++)
					indexStream.write((char*)&records[i], sizeof(records[i]));
				for (size_t i = 0; i < fileInfos.size(); i++)
					indexStream.write((char*)&fileInfos[i], sizeof(fileInfos[i]));
				for (size_t i = 0; i < names.size(); i++)
					indexStream.write(names[i].c_str(), names[i].length() + 1);
			}
		private:
			void _visit(ArchiveNode* node, std::ostream& archiveStream, int recordIndex, int &filesOffset, int& recordCounter, int& infosCounter, int directoryOffset, int& namesIndex)
			{
				if (node->isFile())
				{
					return;
				}

				//create directory record
				Record record = { 0 };

				record.id = recordCounter; //id of this record
				record.parent_record_id = recordIndex;
				

				record.directory_info_offset = directoryOffset; //-1;

				record.number_of_fileinfos = node->size();
				record.unknown_1 = node->size();

				record.fileinfos_offset = record.number_of_fileinfos==0? -1 : infosCounter;


				records.push_back(record);

				//iterate through the children of this directory
				for (size_t i = 0; i < node->size(); i++)
				{
					ArchiveNode* child = node->getChild(i);

					//whatever this child is, we need to construct its file info.
					FileInfo fileInfo = { 0 };
					fileInfo.unknown = 0x01;
					fileInfo.record_id = recordCounter; //containing record id
					fileInfo.is_file = child->isFile();

					fileInfo.filename_offset = namesIndex;			//index of this element name
					fileInfo.filename_length = child->getName().length();

					
					//add name
					names.push_back(child->getName());
					namesIndex += child->getName().length() + 1;

					if (child->isFile())
					{
						TBuildFilter filter;
						std::unique_ptr<NodeSource> source = child->createNodeSource();
						fileInfo.file_real_size=filter(source.get(), child, reinterpret_cast<char*>(fileInfo.sha_256), archiveStream);

						fileInfo.file_full_size = child->isFile() ? (fileInfo.file_real_size / 0x800 + 1) * 0x800 : 0;

						fileInfo.file_offset = filesOffset;
						filesOffset += fileInfo.file_full_size;

						//pad file
						if (fileInfo.file_full_size>fileInfo.file_real_size)
						{
							int pad = fileInfo.file_full_size - fileInfo.file_real_size;
							char buf[0x800] = { 0 };
							//source->read(buf, pad);
							archiveStream.write(buf, pad);
						}

						//header.archive_full_size += fileInfo.file_full_size;

					}

					fileInfos.push_back(fileInfo);
				}

				recordCounter++;
				int baseOff = infosCounter;
				infosCounter += node->size();

				for (size_t i = 0; i < node->size(); i++,header.number_of_files++)
				{
					_visit(node->getChild(i), archiveStream, record.id, filesOffset, recordCounter, infosCounter, baseOff+i, namesIndex);
				}

			}

		private:
			std::ostream& indexStream;

			ff4psp::impl::Header header;
			std::vector<ff4psp::impl::Record> records;
			std::vector<ff4psp::impl::FileInfo> fileInfos;
			std::vector<std::string> names;
		};
	}
}