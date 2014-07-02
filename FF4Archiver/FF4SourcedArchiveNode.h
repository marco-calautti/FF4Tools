#pragma once

#include "FF4ArchiveNode.h"
#include "FF4NodeSource.h"

#include <memory>

namespace ff4psp
{
	namespace impl
	{

		class SourcedArchiveNode : public ArchiveNode
		{

		public:

			virtual std::unique_ptr<NodeSource> getSource() const = 0;

			virtual void setSource(const std::string& filename) = 0;

			virtual const std::string& getSourceName() const = 0;
		};
	}
}