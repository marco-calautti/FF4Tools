#pragma once

#include "FF4SourcedArchiveNode.h"
#include "FF4Internals.h"

#include <string>

namespace ff4psp
{
	namespace impl
	{
		class ArchiveNodeImpl : public SourcedArchiveNode
		{

		public:

			ArchiveNodeImpl();

			ArchiveNodeImpl(const ff4psp::impl::FileInfo& info, const std::string& name, const std::string& srcName,
				ArchiveNode* parent);

			virtual std::unique_ptr<NodeSource> getSource() const 
			{ 
				std::unique_ptr<NodeSource> p(new NodeSource(sourceName,offset,getFileSize())); 
				return std::move(p);
			}

			virtual void setSource(const std::string& filename) { sourceName=filename; }

			virtual const std::string& getSourceName() const { return sourceName; }

			virtual void setName(const std::string& n) { name=n; }

			virtual void setIsFile(bool f) { isfile=f; }

			virtual void setFileSize(long s) { filesize=s; }

			virtual void setChecksum(const std::string& c) { checksum=c; }

			virtual void addChild(std::unique_ptr<ArchiveNode> n) { children.push_back(std::move(n)); }

			virtual void setChild(size_t i, std::unique_ptr<ArchiveNode> n) { children[i]=std::move(n); }

			virtual void setParent(const ArchiveNode* p)  { parent=const_cast<ArchiveNode*>(p); }

		private:
			std::string sourceName;
			long offset;
		};
	}
}