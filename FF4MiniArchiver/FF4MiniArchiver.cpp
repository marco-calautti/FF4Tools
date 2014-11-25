#include <iostream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/locale.hpp>

#include "types.h"
#include "FF4Utils.h"
#include "FF4Exceptions.h"
#include "Lzss.h"

typedef enum
{
	NORMAL_FILE = 0x00,
	MODEL_FILE = 0X01,
	IMAGE_FILE = 0x03,
	BINARY_FILE = 0x65

}FileType;

const std::string NORMAL_STRING = "normal";
const std::string MODEL_STRING = "model";
const std::string IMAGE_STRING = "img";
const std::string COMPIMAGE_STRING = "compimg";
const std::string BINARY_STRING = "binary";

typedef struct
{
	uint16_t number_of_files;
	uint16_t file_type;
	uint32_t file_offset;
	uint32_t file_size;
	uint32_t index;
	char file_name[0x30];
}MiniRecord;

void extract(const std::string& archivePath, const std::string& outDirectory)
{
	boost::filesystem::path inputPath = ff4psp::utils::encodings::path(archivePath);

	boost::filesystem::path outPathDir = ff4psp::utils::encodings::path(outDirectory);

	if (!boost::filesystem::exists(outPathDir))
		boost::filesystem::create_directories(outPathDir);

	if (!boost::filesystem::is_directory(outPathDir))
		ff4psp::FF4Exception::raise("Not a valid directory!");

	boost::filesystem::ifstream inputStream(inputPath, std::ios::in | std::ios::binary);

	if (!inputStream)
		ff4psp::FF4Exception::raise("Cannot open archive file!");

	int size=boost::filesystem::file_size(inputPath);
	char* buffer = new char[size];
	inputStream.read(buffer, size);
	inputStream.close();


	MiniRecord* records = (MiniRecord*)buffer;
	uint16_t total = records->number_of_files;

	for (uint16_t i = 0; i < total; i++)
	{
		if (records[i].index != i)
			ff4psp::FF4Exception::raise(boost::format("Record index %i mismatch!") % i);

		std::string filename;
		bool compressed = false;
		std::string stringType = "";
		switch (records[i].file_type)
		{
		case FileType::NORMAL_FILE:
			stringType = NORMAL_STRING;
			break;
		case FileType::MODEL_FILE:
			stringType = MODEL_STRING;
			break;
		case FileType::IMAGE_FILE:
			compressed = strncmp("LZTX", buffer + records[i].file_offset,4) == 0;
			stringType = compressed? COMPIMAGE_STRING : IMAGE_STRING;
			break;
		case FileType::BINARY_FILE:
			stringType = BINARY_STRING;
			break;
		default:
			ff4psp::FF4Exception::raise(boost::format("Unknown file type: %d.") % records[i].file_type);
			break;
		}

		filename = (boost::format("%04i_" + stringType + "_" + records[i].file_name) % i).str();
		std::string utf8_filename = boost::locale::conv::to_utf<char>(filename, "Shift-JIS");

		boost::filesystem::path p=ff4psp::utils::encodings::path(utf8_filename);
		boost::filesystem::path outPath = outPathDir / p;

		boost::filesystem::ofstream outputStream(outPath, std::ios::out | std::ios::binary);
		if (records[i].file_type != FileType::IMAGE_FILE || !compressed)
		{
			outputStream.write(&buffer[records[i].file_offset], records[i].file_size);
		}
		else
		{
			uint8_t* data = nullptr;
			int size = 0;
			size = ff4psp::compression::LzssDecode(data, (uint8_t*)buffer + records[i].file_offset + 4, records[i].file_size - 4);
			outputStream.write((char*)data, size);
			delete[] data;
		}

	}

	delete[] buffer;
}

FileType getFileType(const std::string& typeString)
{
	if (typeString == NORMAL_STRING)
		return FileType::NORMAL_FILE;
	else if (typeString == IMAGE_STRING || typeString == COMPIMAGE_STRING)
		return FileType::IMAGE_FILE;
	else if (typeString == MODEL_STRING)
		return FileType::MODEL_FILE;
	else if (typeString == BINARY_STRING)
		return FileType::BINARY_FILE;
	else
		ff4psp::FF4Exception::raise(boost::format("Illegal type string %s!") % typeString);

	return FileType::NORMAL_FILE;
}

bool getInfo(const std::string& name, int& index, std::string& typeString, std::string& filename)
{
	
	std::string regex = "(\\d+)_(";
	regex += NORMAL_STRING + "|";
	regex += BINARY_STRING + "|";
	regex += IMAGE_STRING + "|";
	regex += COMPIMAGE_STRING + "|";
	regex += MODEL_STRING + ")";
	regex += "_(.+)";

	boost::regex exp(regex);

	boost::smatch match;
	if (!boost::regex_search(name, match, exp))
		return false;

	index = atoi(std::string(match[1].first, match[1].second).c_str());
	typeString = std::string(match[2].first, match[2].second);
	filename = std::string(match[3].first, match[3].second);

	return true;
}

void create(const std::string& archive, const std::string& inputDir)
{
	if (!boost::filesystem::exists(inputDir) || !boost::filesystem::is_directory(inputDir))
		ff4psp::FF4Exception::raise("Not a valid directory!");

	boost::filesystem::path pathDir = ff4psp::utils::encodings::path(inputDir);

	std::vector<boost::filesystem::path> files;
	for (boost::filesystem::directory_iterator it(pathDir); it != boost::filesystem::directory_iterator(); it++)
	{
		if (!boost::filesystem::is_regular_file(it->status()))
			continue;
		files.push_back(*it);
	}

	std::sort(files.begin(), files.end());

	

	boost::filesystem::path outPath = ff4psp::utils::encodings::path(archive);
	boost::filesystem::ofstream outStream(outPath, std::ios::binary | std::ios::out);

	uint32_t curOffset = files.size()*sizeof(MiniRecord);
	outStream.seekp(curOffset);
	std::vector<MiniRecord> records;
	int numFiles = 0;
	for (size_t i = 0; i < files.size(); i++)
	{
		std::string name = ff4psp::utils::encodings::path_string(files[i].filename());

		int index;
		std::string typeString, filename;

		if (!getInfo(name, index, typeString, filename))
			continue; //ff4psp::FF4Exception::raise(boost::format("Not valid file name %s!") % name);
		
		if (index != numFiles)
			ff4psp::FF4Exception::raise(boost::format("Missing file with index %d!") % numFiles);

		MiniRecord record = { 0 };

		//if (i == 0)
			//record.number_of_files = files.size();

		record.index = index;
		std::string shift_jis_string = boost::locale::conv::from_utf<char>(filename, "Shift-JIS");

		strcpy(record.file_name, shift_jis_string.c_str());
		record.file_offset = curOffset;
		record.file_type = getFileType(typeString);

		boost::filesystem::ifstream inStream(files[i], std::ios::binary|std::ios::in);
		long size=boost::filesystem::file_size(files[i]);

		char* buf = new char[size];
		inStream.read(buf, size);
		
		if (typeString == COMPIMAGE_STRING)
		{
			char* data = nullptr;
			int dataSize = ff4psp::compression::LzssEncode((uint8_t*&)data, (uint8_t*)buf, size);
			outStream.write("LZTX", strlen("LZTX"));
			outStream.write(data, dataSize);
			size = dataSize + 4;;
			delete[] data;
		}
		else
			outStream.write(buf, size);

		delete[] buf;

		record.file_size = size;

		int pad = ((size % 0x20) == 0) ? 0 : 0x20 - size % 0x20;

		char padbuf[0x20] = { 0 };
		outStream.write(padbuf, pad);

		curOffset += size;
		curOffset += pad;

		records.push_back(record);
		numFiles++;	//just count the right files in this directory
	}

	records[0].number_of_files = numFiles;

	outStream.seekp(0);
	for (size_t i = 0; i < records.size(); i++)
		outStream.write((char*)&records[i], sizeof(records[i]));


}

void PrintUsage()
{
	std::cout << "MiniArchiver: [-extract/-create] [archive] [directory]" << std::endl;
}

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		PrintUsage();
		return 0;
	}

	try
	{

		if (strcmp("-extract", argv[1]) == 0)
		{
			extract(argv[2], argv[3]);
		}
		else if (strcmp("-create", argv[1]) == 0)
		{
			create(argv[2], argv[3]);
		}
		else
			std::cout << "Invalid parameter!" << std::endl;
	}
	catch (const ff4psp::FF4Exception& ex)
	{
		std::cout << ex.what()<<std::endl;
	}

	return 0;
}