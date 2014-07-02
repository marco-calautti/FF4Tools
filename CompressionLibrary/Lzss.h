#pragma once

#include "types.h"
  
#define N        4096   /* size of ring buffer */
#define F          18   /* upper limit for match_length */
#define THRESHOLD   2   /* encode string into position and length
                           if match_length is greater than this */
#define NIL         N   /* index for root of binary search trees */
  
namespace ff4psp
{
	namespace compression
	{
		int LzssEncode(uint8_t* &dest, uint8_t* src, int src_size);
		int LzssDecode(uint8_t* &dest, uint8_t* src, int src_size); 
	}
}
