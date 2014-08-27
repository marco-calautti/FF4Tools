#pragma once

#include "FF4NodeSource.h"
#include "FF4ArchiveNode.h"

#include <string>

namespace ff4psp
{
	namespace impl
	{
		class NormalExtractFilter
		{
			public:
				void operator()(NodeSource* source, const std::string& path);
		};

		class DecompressFilter
		{
			public:
				void operator()(NodeSource* source, const std::string& path);
		};

		class NormalBuildFilter
		{
		public:
			int operator()(NodeSource* source, const ArchiveNode* node, char checksum[ArchiveNode::CHECKSUM_SIZE], std::ostream& outputStream);
		};

		class CompressFilter
		{
		public:
			int operator()(NodeSource* source, const ArchiveNode* node, char checksum[ArchiveNode::CHECKSUM_SIZE], std::ostream& outputStream);
		};
	}
}