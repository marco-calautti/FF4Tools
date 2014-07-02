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

		virtual S visit(const ArchiveNode* node, const T& state)=0;
	};

	namespace impl
	{
		template <typename TFilter>
		class ExtractionVisitor : public ArchiveVisitor<void,std::string>
		{

		public:
			ExtractionVisitor() { }

			
			virtual void visit(const ArchiveNode* node, const std::string& state)
			{
				if(node->isFile())
				{
					boost::filesystem::path filePath = ff4psp::utils::encodings::path(state);
					filePath = filePath / node->getName();
		
					std::unique_ptr<NodeSource> source=dynamic_cast<SourcedArchiveNode*>(const_cast<ArchiveNode*>(node))->getSource();

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
					visit(node->getChild(i),fullPath.string());
				}
			}
		};

		class NodeRetrieveVisitor : public ArchiveVisitor<ArchiveNode*,std::string>
		{
		public:
			virtual ArchiveNode* visit(const ArchiveNode* node, const std::string& state);

		private:
			bool isValidPath(const std::string& path) const;
		};

		class ImportVisitor : public ArchiveVisitor<std::unique_ptr<ArchiveNode>, std::string>
		{
		public:

			virtual std::unique_ptr<ArchiveNode> visit(const ArchiveNode* node,const std::string& inputDirectory);

		private:

			void _visit(const ArchiveNode* original, ArchiveNodeImpl* current, const boost::filesystem::path& inputPath);

			/*std::unique_ptr<ArchiveNode> _visit(const ArchiveNode* current, 
														const std::shared_ptr<ff4psp::ArchiveNode>& node, 
														const boost::filesystem::path& inputDirectory,
														long& delta);*/

			//void cleanup_and_throw(const std::string& err);

			//std::string indexFile;
			//std::string dataFile;
		};
	}
}