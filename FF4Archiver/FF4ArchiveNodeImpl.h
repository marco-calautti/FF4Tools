#pragma once

#include "FF4Internals.h"
#include "FF4ArchiveNode.h"
#include <string>

namespace ff4psp
{
	namespace impl
	{
		class ArchiveNodeImpl : public ArchiveNode
		{

		public:

			ArchiveNodeImpl();

			ArchiveNodeImpl(const ff4psp::impl::FileInfo& info, const std::string& name, const std::string& srcName,
				ArchiveNode* parent);


			virtual void setName(const std::string& n) { name = n; }

			virtual void setIsFile(bool f) { isfile = f; }

			//virtual void setChecksum(const std::string& c) { checksum=c; }

			virtual void getChecksum(char checksum[CHECKSUM_SIZE]) const;

			virtual void addChild(std::unique_ptr<ArchiveNode> n) { children.push_back(std::move(n)); }

			virtual void setChild(size_t i, std::unique_ptr<ArchiveNode> n) { children[i] = std::move(n); }

			virtual void setParent(const ArchiveNode* p)  { parent = const_cast<ArchiveNode*>(p); }

			virtual void setSourceFile(const std::string& filename, long off, long size, NodeSource::SourceType type);

			virtual std::unique_ptr<ff4psp::NodeSource> createNodeSource() const;

		private:
			mutable bool recomputeChecksum = false;
			char checksum[CHECKSUM_SIZE];
			std::string sourceFilename;
			long offset;
			NodeSource::SourceType type;
		};
	}
}