#pragma once

#include <vector>
#include <string>
#include <memory>

#include <boost/filesystem.hpp>

#include "FF4Utils.h"
#include "FF4NodeSource.h"
#include "FF4Internals.h"

namespace ff4psp
{

	class ArchiveNode
	{
		friend class FF4Archive;

	protected:
		std::string name;
		long filesize;
		bool isfile;

		ArchiveNode(){}
		ArchiveNode(const ArchiveNode& n) {}
		ArchiveNode& operator=(const ArchiveNode& n) { return *this; }
		
	public:
		static const int CHECKSUM_SIZE = ff4psp::impl::CHECKSUM_SIZE;

		virtual ~ArchiveNode() {}

		virtual const std::string& getName() const { return name; }

		virtual bool isFile() const { return isfile; }

		virtual long getFileSize() const { return filesize; }
		
		virtual void getChecksum(char checksum[CHECKSUM_SIZE]) const = 0;

		virtual size_t size() const { return children.size(); }

		virtual ArchiveNode* getChild(size_t index) const { return children[index].get(); }

		virtual ArchiveNode* getParent() const { return parent; }

		virtual bool isRoot() const {return parent==nullptr; }
		
		virtual void setSourceFile(const std::string& filename, long off=0, long size=-1, NodeSource::SourceType type=NodeSource::SourceType::File)=0;
		
		virtual std::unique_ptr<ff4psp::NodeSource> createNodeSource() const = 0;

		protected:
			std::vector<std::unique_ptr<ArchiveNode>> children;
			ArchiveNode* parent;
	};
}
