#include <iostream>
#include <string>
#include <fstream>

#include "Lzss.h"

void PrintUsage()
{
	std::cout<<"FF4CC file DeCompressor - (C) 2014 Phoenix - SadNES cITy Translations"<<std::endl;
	std::cout<<"Usage: -c/-d inputfile outputfile"<<std::endl;
}

int main(int argc, char* argv[])
{
	if(argc!=4)
	{
		PrintUsage();
		return 0;
	}

	std::string command(argv[1]);

	std::ifstream input(argv[2],std::ios::in|std::ios::binary);
	std::ofstream output(argv[3],std::ios::out|std::ios::binary);

	int (*func)(uint8_t*&,uint8_t *, int);

	if(command=="-c")
	{
		func=ff4psp::compression::LzssEncode;
	}else if(command=="-d")
	{
		func=ff4psp::compression::LzssDecode;
	}else
	{
		std::cout<<"Unknown command"<<command<<std::endl;
		return -1;
	}

	input.seekg(0, std::ios::end);
    std::streamoff size = input.tellg();
	input.seekg(0);

	uint8_t* buffer=new uint8_t[(unsigned int)size];
	input.read((char*)buffer,size);
	input.close();

	uint8_t* dest;
	int destSize=func(dest,buffer,(int)size);
	output.write((char*)dest,destSize);
	output.close();
	return 0;
}