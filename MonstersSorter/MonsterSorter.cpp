#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

#include "FF4Utils.h"
#include "FF4Exceptions.h"

typedef struct
{
	std::streamoff start;
	int entries;
}ListData;

typedef struct
{
	uint16_t name_pointer;
	uint8_t unknown1[0x14];
	uint16_t list_position;
	uint8_t unknown2[6];

}Entry;

typedef struct
{
	int name_pos;
	int original_pos;
} SortEntry;

const ListData FF4_LIST_OFFSETS_EU[] = { { 0x272208, 0x103 }, { 0x274062, 0x16B }, { 0x276AEC, 0x49 } }; //EUR version, FF4, TAY, INTERLUDE
const ListData FF4_LIST_OFFSETS_US[] = { { 0x272218, 0x103 }, { 0x274072, 0x16B }, { 0x276AFC, 0x49 } }; //US version, FF4, TAY, INTERLUDE
const int SIZE = 3;

const std::streamoff MESSAGE_MONSTERS_START = 0xFAD;
const int TOTAL_MONSTERS = 0x280;

void PrintUsage()
{
	std::cout << "MonstersSorter: <EBOOT.BIN> <message_en.lzs> [-eur/-usa]" << std::endl;
}

int stringCompare(const std::string& first, const std::string& second)
{
	for (size_t i = 0; i < first.length() && i < second.length(); i++)
	{
		if (first[i] < second[i])
			return -1;
		else if (first[i] > second[i])
			return 1;
	}

	return first.length() - second.length();
}

void readStrings(boost::filesystem::ifstream& stream, std::vector<std::string>& strings)
{
	stream.seekg(MESSAGE_MONSTERS_START*4);
	uint32_t pointer;
	stream.read((char*)&pointer, 4);
	stream.seekg(pointer);

	for (int i = 0; i < TOTAL_MONSTERS; i++)
	{
		std::string monster;
		std::getline(stream, monster, '\0');
		strings.push_back(monster);
	}
}

void sort(std::vector<Entry>& entries, std::vector<std::string>& strings)
{
	std::vector<SortEntry> list;

	for (size_t i = 0; i < entries.size(); i++)
	{
		SortEntry entry;
		entry.name_pos = entries[i].name_pointer;
		if (entry.name_pos == 343)
			std::cout << "";
		entry.original_pos = i;
		list.push_back(entry);
	}

	for (size_t i = 1; i <= list.size(); i++)
	{
		for (size_t j = 0; j < list.size() - 1; j++)
		{
			std::string& first = strings[list[j].name_pos];
			std::string& second = strings[list[j + 1].name_pos];

			if (stringCompare(first,second)>0)
				std::swap(list[j], list[j + 1]);
		}
	}

	for (size_t i = 0; i < list.size(); i++)
	{
		entries[list[i].original_pos].list_position = i + 1;
	}

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

		const ListData* offsets = nullptr;

		if (strcmp(argv[3], "-eur") == 0)
			offsets = FF4_LIST_OFFSETS_EU;
		else if (strcmp(argv[3], "-usa") == 0)
			offsets = FF4_LIST_OFFSETS_US;
		else
			ff4psp::FF4Exception::raise(boost::format("Illegal parameter %s!") % argv[3]);

		boost::filesystem::fstream ebootStream(ff4psp::utils::encodings::path(argv[1]),
			std::ios::in | std::ios::out | std::ios::binary);

		if (!ebootStream)
			ff4psp::FF4Exception::raise(boost::format("Cannot open file %s") % argv[1]);

		boost::filesystem::ifstream messageStream(ff4psp::utils::encodings::path(argv[2]), std::ios::in | std::ios::binary);

		if (!messageStream)
			ff4psp::FF4Exception::raise(boost::format("Cannot open file %s") % argv[2]);

		std::vector<std::string> strings;
		readStrings(messageStream, strings);
		messageStream.close();

		for (int i = 0; i < SIZE; i++)
		{
			ebootStream.seekp(offsets[i].start);
			std::vector<Entry> entries;

			for (int j = 0; j < offsets[i].entries; j++)
			{
				Entry entry;
				ebootStream.read((char*)&entry, sizeof(entry));
				entries.push_back(entry);
			}

			sort(entries, strings);
			ebootStream.seekp(offsets[i].start);

			for (int j = 0; j < offsets[i].entries; j++)
			{
				ebootStream.write((char*)&entries[j], sizeof(entries[j]));
			}
		}

		ebootStream.close();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}