#pragma once

#include "types.h"

namespace ff4psp
{
	namespace impl
	{
		const int CHECKSUM_SIZE=32;
		//const char DEFAULT_SHA256[] = { 0xE3, 0xB0, 0xC4, 0x42, 0x98, 0xFC, 0x1C, 0x14, 0x9A, 0xFB, 0xF4, 0xC8, 0x99, 0x6F, 0xB9, 0x24,
										//0x27, 0xAE, 0x41, 0xE4, 0x64, 0x9B, 0x93, 0x4C, 0xA4, 0x95, 0x99, 0x1B, 0x78, 0x52, 0xB8, 0x55 };
		typedef struct
		{
			uint32_t records;
			uint32_t number_of_files;
			uint32_t names_table_size;
			uint32_t archive_full_size;
			char pad[0x10];
		} Header;

		typedef struct
		{
			uint32_t id;
			uint32_t parent_record_id;
			uint32_t fileinfos_offset; // the index of the first fileinfo of this record, -1 if number_of_fileinfos=0
			uint32_t number_of_fileinfos; //number of file infos in this record
			uint32_t unknown_1; //seems to be always equal to number_of_fileinfos
			uint32_t directory_info_offset; //the index of the file info of this directory record (-1 if this record is the root)
			uint8_t unknown_2[8]; //non-zero only for "menu", seems to be not used by the game.
		} Record;

		typedef struct
		{
			uint16_t unknown; //zero for data and sound directories, may 0 if it is a directory and has no files inside
			uint16_t is_file;
			uint32_t filename_offset;
			uint32_t filename_length;
			uint32_t file_offset;
			uint32_t file_real_size;
			uint32_t pad; //it is always 0
			uint32_t record_id;
			uint32_t file_full_size;
			uint8_t sha_256[CHECKSUM_SIZE];

		} FileInfo;
	}
}