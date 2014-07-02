#pragma once

#include <boost/filesystem/fstream.hpp>

namespace ff4psp
{
	namespace impl
	{
		class NodeSource
		{
		public:
			NodeSource(const std::string& name, long off, long s);

		private:
			NodeSource(const NodeSource& src){}
			
			NodeSource& operator=(const NodeSource& src){ return *this; }

		public:
			std::istream& getStream() { return stream; }

			long getSize() const { return size; }

		private:
			std::string filename;
			boost::filesystem::ifstream stream;
			long offset;
			long size;
		};
	}
}