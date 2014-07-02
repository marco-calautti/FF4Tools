#pragma once

#include "FF4NodeSource.h"

#include <string>

namespace ff4psp
{
	namespace impl
	{
		class NormalFilter
		{
			public:
				void operator()(NodeSource* source, const std::string& path);
		};

		class DecompressFilter
		{
			public:
				void operator()(NodeSource* source, const std::string& path);
		};
	}
}