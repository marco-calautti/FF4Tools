#pragma once

#include <boost/filesystem/fstream.hpp>

namespace ff4psp
{
		class NodeSource
		{
		public:
			typedef enum{Original,File} SourceType;

			NodeSource(const std::string& name, long off, long s,SourceType type);

		private:
			NodeSource(const NodeSource& src){}
			
			NodeSource& operator=(const NodeSource& src){ return *this; }

		public:
			size_t read(char* buffer, size_t count);

			long getSize() const { return size; }

			SourceType getType(){ return type;  }

		private:
			std::string filename;
			boost::filesystem::ifstream stream;
			long offset;
			long size;
			SourceType type;
		};
}