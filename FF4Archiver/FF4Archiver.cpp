#include "FF4Archive.h"
#include "FF4ArchiveNode.h"
#include "FF4Exceptions.h"

#include <iostream>
#include <string>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

void printUsage()
{
	std::cout << "PAC Archive manager for FF4CC - Phoenix, SadNES cITy Translations" << std::endl << std::endl;
	std::cout << "Usage: ";
	std::cout << "\t-extract index_file data_file out_directory" << std::endl;
	std::cout << "\t-import index_file data_file import_list" << std::endl << std::endl;
	std::cout << "where import_list: ";
	std::cout << "dest_path=source_path[,dest_path=source_path]" << std::endl;
	std::cout << "ex. /data/message=c:\\FF4\\data\\message,/data/title.lzs=c:\\FF4\\data\\title.lzs" << std::endl;
}

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 5)
		{
			printUsage();
			return 0;
		}
		std::string message = "Illegal parameter ";
		if (strcmp(argv[1], "-extract") == 0)
		{
			std::cout << "Opening archive..." << std::endl;
			ff4psp::FF4Archive archive(argv[2], argv[3]);

			if (!boost::filesystem::exists(argv[4]))
				boost::filesystem::create_directories(argv[4]);

			std::cout << "Extracting files, please wait..." << std::endl;
			archive.extract(archive.getRoot(), argv[4]);
			std::cout << "Extraction complete!" << std::endl;
		}
		else if (strcmp(argv[1], "-import") == 0)
		{
			
			std::vector<std::string> importStrings;
			boost::split(importStrings, argv[4], boost::is_any_of(","));

			if (importStrings.size() == 0)
				ff4psp::FF4Exception::raise((message += argv[4]).c_str());

			std::cout << "Opening archive..." << std::endl;
			ff4psp::FF4Archive archive(argv[2], argv[3]);

			std::cout << "Parsing import list..." << std::endl;
			for (size_t i = 0; i < importStrings.size(); i++)
			{
				std::string str = importStrings[i];
				std::vector<std::string> pair;
				boost::split(pair, str, boost::is_any_of("="));
				if (pair.size() != 2)
					ff4psp::FF4Exception::raise((message += str).c_str());

				ff4psp::ArchiveNode* node = archive.getNodeFromPath(pair[0]);
				archive.import(node, pair[1]);
			}

			std::cout << "Rebuilding archive, please wait..." << std::endl;
			boost::filesystem::path tempIndex = boost::filesystem::unique_path("%%%%_%%%%.tmp");
			tempIndex = ff4psp::utils::encodings::path(argv[2]).parent_path() / tempIndex;

			boost::filesystem::path tempArchive = boost::filesystem::unique_path("%%%%_%%%%.tmp");
			tempArchive = ff4psp::utils::encodings::path(argv[3]).parent_path() / tempArchive;

			archive.build(ff4psp::utils::encodings::path_string(tempIndex), ff4psp::utils::encodings::path_string(tempArchive));

			boost::filesystem::rename(tempIndex, argv[2]);
			boost::filesystem::rename(tempArchive, argv[3]);
			std::cout << "Build complete!" << std::endl;
		}
		else
		{
			message += argv[1];
			ff4psp::FF4Exception::raise(message.c_str());
		}

		/*
		ff4psp::FF4Archive archive("e:\\temp\\PAC0.bin","e:\\temp\\PAC1.bin");

		ff4psp::ArchiveNode* root=archive.getRoot();

		ff4psp::ArchiveNode* node=archive.getNodeFromPath("data/menu/monster/ms_005.lzs");

		//archive.extract(root,"e:\\temp\\FF4");

		archive.import(node,"e:\\temp\\PAC\\data\\menu\\monster\\ms_005.lzs");

		archive.build("e:\\temp\\PAC\\");*/
	}
	catch (const ff4psp::FF4Exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}catch(const std::exception& ex)
	{
		std::cout<<ex.what()<<std::endl;
	}
	return 0;
}
