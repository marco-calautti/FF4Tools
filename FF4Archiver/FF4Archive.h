#pragma once

#include <string>

#include <boost/shared_array.hpp>

#include "FF4Internals.h"
#include "FF4ArchiveNodeImpl.h"

namespace ff4psp
{
	class ArchiveNode;

	class FF4Archive
	{
	public:

		static const std::string DEFAULT_INDEX_FILENAME;
		static const std::string DEFAULT_DATA_FILENAME;

		/**
		* Constructs the archive sarting from a file on disk. If the file does not exists, it creates a new one.
		*/
		FF4Archive(const std::string& PAC0,
				   const std::string& PAC1);

		/**
		* Returns the root node of this archive. If the archive is empty, the root has no children.
		*/
		const ArchiveNode* getRoot() const { return root.get(); }

		/**
		* Returns the ArchiveNode rooted in the directory/file "path". A path is of the form "data/monster/ms_01.lzs", or "data/message". If the given path does not exist in the archive, NULL is returned.
		*/
		const ArchiveNode* getNodeFromPath(const std::string& path) const;

		/**
		* Extracts in "outputDirectory" all the files inside the ArchiveNode "path". If "path" is a file, it just extracts the file. Note that if "path" refers to a directory "dir", the extraction tree
		will not be rooted in "dir".
		*/
		void extract(const ArchiveNode* path,
					 const std::string& outputDirectory) const;

		/**
		* Imports the directory tree rooted in "inputDirectory" (except for the root "inputDirectory" itself) into the ArchiveNode "path". Returns the new node after the import.
		*/
		const ArchiveNode* import(const ArchiveNode* path,
					const std::string& inputDirectory);

	private:

		void constructRootNode(ff4psp::impl::ArchiveNodeImpl* node,
							   const boost::shared_array<ff4psp::impl::Record>& records,
							   int& rIndex,
							   const boost::shared_array<ff4psp::impl::FileInfo>& files,
							   const boost::shared_array<char>& names_table);

		bool isValidPath(const std::string& path) const;

		std::unique_ptr<ff4psp::impl::ArchiveNodeImpl> root;
		std::string indexFile;
		std::string dataFile;
	};
}