#pragma once

#include <vector>
#include <string>
#include <memory>

namespace ff4psp
{

	class ArchiveNode
	{
		friend class FF4Archive;

	protected:
		std::string name;
		std::string checksum;
		long filesize;
		bool isfile;
		
		ArchiveNode(){}
		ArchiveNode(const ArchiveNode& n) {}
		ArchiveNode& operator=(const ArchiveNode& n) { return *this; }
		
	public:

		virtual ~ArchiveNode() {}

		virtual const std::string& getName() const { return name; }

		virtual bool isFile() const { return isfile; }

		virtual long getFileSize() const { return filesize; }
		
		virtual const std::string& getChecksum() const { return checksum; }

		virtual size_t size() const { return children.size(); }

		virtual const ArchiveNode* getChild(size_t index) const { return children[index].get(); }

		virtual const ArchiveNode* getParent() const { return parent; }

		virtual bool isRoot() const {return parent==nullptr; }
		

		protected:
			//long fileoffset;
			std::vector<std::unique_ptr<ArchiveNode>> children;
			ArchiveNode* parent;
	};
}