#pragma once

#include "Defines.h"

namespace PSI {
	
	std::vector<u8> fromBlock(const block &block);

	std::vector<u8> fromU64(const u64 &u);

	u64 toU64(const u8 *data);

	void paddingToBlock(std::vector<u8> &data);

	void aesDecBlocks(AESDec &aesDec, const block* cyphertext, const u64 &blockLength, block* plaintext);

}
