#pragma once

#include "utils.h"

namespace PSI {
	
	std::vector<u8> fromBlock(const block &block) {
		u8* start = (u8*) &block;
		return std::vector<u8>(start, start + sizeof(block));
	}

	std::vector<u8> fromU64(const u64 &u) {
		u8* start = (u8*) &u;
		return std::vector<u8>(start, start + sizeof(u64));
	}

	u64 toU64(const u8 *data) {
		return *((u64*)data);
	}

	void paddingToBlock(std::vector<u8> &data) {
		u64 more = (sizeof(block) - data.size() % sizeof(block)) % sizeof(block);
		for (auto i = 0; i < more; ++i) {
			data.push_back(0);
		}
	}

}
