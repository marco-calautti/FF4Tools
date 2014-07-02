#include "FF4Archive.h"
#include "FF4ArchiveNode.h"
#include "FF4Exceptions.h"

#include <iostream>
#include <string>

#include <boost/program_options.hpp>

int main(int argc, char* argv[])
{
	try
	{
		
		ff4psp::FF4Archive archive("e:\\temp\\PAC0.bin","e:\\temp\\PAC1.bin");

		const ff4psp::ArchiveNode* root=archive.getRoot();

		const ff4psp::ArchiveNode* node=archive.getNodeFromPath("data/menu/monster/ms_005.lzs");

		//archive.extract(root,"e:\\temp\\FF4");

		archive.import(root,"e:\\temp\\FF4\\");
	}catch(const ff4psp::FF4Exception& ex)
	{
		std::cout<<ex.what()<<std::endl;
	}
	return 0;
}
