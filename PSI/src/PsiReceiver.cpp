#include "PsiReceiver.h"

#include <array>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <unordered_map>
#include <iomanip>
#include <bitset>
#include <thread>

namespace PSI {

	void PsiReceiver::run(PRNG& prng, Channel& ch, block commonSeed, const u64& senderSize, const u64& receiverSize, const u64& height, const u64& logHeight, const u64& width, std::vector<block>& receiverSet, const u64& hashLengthInBytes, const u64& h1LengthInBytes, const u64& bucket1, const u64& bucket2) {
	
		Timer timer;
		
		timer.setTimePoint("Receiver start");
		
		TimeUnit start, end;
		
		auto heightInBytes = (height + 7) / 8;
		auto widthInBytes = (width + 7) / 8;
		auto locationInBytes = (logHeight + 7) / 8;
		auto receiverSizeInBytes = (receiverSize + 7) / 8;
		auto shift = (1 << logHeight) - 1;
		auto widthBucket1 = sizeof(block) / locationInBytes;
		
		
		///////////////////// Base OTs ///////////////////////////
		
		IknpOtExtSender otExtSender;
		otExtSender.genBaseOts(prng, ch);
		
		std::vector<std::array<block, 2> > otMessages(width);

		otExtSender.send(otMessages, prng, ch);

		// std::cout << "Receiver base OT finished\n";
		timer.setTimePoint("Receiver base OT finished");
		
		
		
		
		//////////// Initialization ///////////////////
		
		PRNG commonPrng(commonSeed);
		block commonKey;
		AES commonAes;
		
		u8* matrixA[widthBucket1];
		u8* matrixDelta[widthBucket1];
		for (auto i = 0; i < widthBucket1; ++i) {
			matrixA[i] = new u8[heightInBytes];
			matrixDelta[i] = new u8[heightInBytes];
		}

		u8* transLocations[widthBucket1];
		for (auto i = 0; i < widthBucket1; ++i) {
			transLocations[i] = new u8[receiverSize * locationInBytes + sizeof(u32)];
		}
	
		block randomLocations[bucket1];
		

		u8* transHashInputs[width];
		for (auto i = 0; i < width; ++i) {
			transHashInputs[i] = new u8[receiverSizeInBytes];
			memset(transHashInputs[i], 0, receiverSizeInBytes);
		}

		// std::cout << "Receiver initialized\n";
		timer.setTimePoint("Receiver initialized");

		


		/////////// Transform input /////////////////////

		commonPrng.get((u8*)&commonKey, sizeof(block));
		commonAes.setKey(commonKey);
		
		block* recvSet = new block[receiverSize];
		block* aesInput = new block[receiverSize];
		block* aesOutput = new block[receiverSize];
		
		RandomOracle H1(h1LengthInBytes);
		u8 h1Output[h1LengthInBytes];
		
		for (auto i = 0; i < receiverSize; ++i) {
			H1.Reset();
			H1.Update((u8*)(receiverSet.data() + i), sizeof(block));
			H1.Final(h1Output);
			
			aesInput[i] = *(block*)h1Output;
			recvSet[i] = *(block*)(h1Output + sizeof(block));
		}
		
		commonAes.ecbEncBlocks(aesInput, receiverSize, aesOutput);
		for (auto i = 0; i < receiverSize; ++i) {
			recvSet[i] ^= aesOutput[i];
		}
		
		// std::cout << "Receiver set transformed\n";
		timer.setTimePoint("Receiver set transformed");
		


		
		
		for (auto wLeft = 0; wLeft < width; wLeft += widthBucket1) {
			auto wRight = wLeft + widthBucket1 < width ? wLeft + widthBucket1 : width;
			auto w = wRight - wLeft;

			
			//////////// Compute random locations (transposed) ////////////////
			
			commonPrng.get((u8*)&commonKey, sizeof(block));
			commonAes.setKey(commonKey);

			for (auto low = 0; low < receiverSize; low += bucket1) {
			
				auto up = low + bucket1 < receiverSize ? low + bucket1 : receiverSize;

				commonAes.ecbEncBlocks(recvSet + low, up - low, randomLocations); 
					
				for (auto i = 0; i < w; ++i) {
					for (auto j = low; j < up; ++j) {
						memcpy(transLocations[i] + j * locationInBytes, (u8*)(randomLocations + (j - low)) + i * locationInBytes, locationInBytes);
					}
				}
			}
		
		

			//////////// Compute matrix Delta /////////////////////////////////
			
			for (auto i = 0; i < widthBucket1; ++i) {
				memset(matrixDelta[i], 255, heightInBytes);
			}
			
			for (auto i = 0; i < w; ++i) {
				for (auto j = 0; j < receiverSize; ++j) {
					auto location = (*(u32*)(transLocations[i] + j * locationInBytes)) & shift;
					
					matrixDelta[i][location >> 3] &= ~(1 << (location & 7));
				}
			}
			
			
			
			//////////////// Compute matrix A & sent matrix ///////////////////////

			u8* sentMatrix[w];
			
			for (auto i = 0; i < w; ++i) {
				PRNG prng(otMessages[i + wLeft][0]);
				prng.get(matrixA[i], heightInBytes);
				
				sentMatrix[i] = new u8[heightInBytes];
				prng.SetSeed(otMessages[i + wLeft][1]);
				prng.get(sentMatrix[i], heightInBytes);
				
				for (auto j = 0; j < heightInBytes; ++j) {
					sentMatrix[i][j] ^= matrixA[i][j] ^ matrixDelta[i][j];
				}
				
				ch.asyncSend(sentMatrix[i], heightInBytes);
			}
			
			
			
			///////////////// Compute hash inputs (transposed) /////////////////////
	
			for (auto i = 0; i < w; ++i) {
				for (auto j = 0; j < receiverSize; ++j) {
					auto location = (*(u32*)(transLocations[i] + j * locationInBytes)) & shift;
					
					transHashInputs[i + wLeft][j >> 3] |= (u8)((bool)(matrixA[i][location >> 3] & (1 << (location & 7)))) << (j & 7);
				}		
			}
			
		}
		
		std::cout << "Receiver matrix sent and transposed hash input computed\n";
		timer.setTimePoint("Receiver matrix sent and transposed hash input computed");

		
		
		
		/////////////////// Compute hash outputs ///////////////////////////
		
		RandomOracle H(hashLengthInBytes);
		u8 hashOutput[sizeof(block)];
		std::unordered_map<u64, std::vector<std::pair<block, u32>>> allHashes;
		u8* hashInputs[bucket2];
		for (auto i = 0; i < bucket2; ++i) {
			hashInputs[i] = new u8[widthInBytes];
		}
		
		for (auto low = 0; low < receiverSize; low += bucket2) {
			auto up = low + bucket2 < receiverSize ? low + bucket2 : receiverSize;
			
			for (auto j = low; j < up; ++j) {
				memset(hashInputs[j - low], 0, widthInBytes);
			}
			
			for (auto i = 0; i < width; ++i) {
				for (auto j = low; j < up; ++j) {
					hashInputs[j - low][i >> 3] |= (u8)((bool)(transHashInputs[i][j >> 3] & (1 << (j & 7)))) << (i & 7);
				}
			}
			
			for (auto j = low; j < up; ++j) {
				H.Reset();
				H.Update(hashInputs[j - low], widthInBytes);
				H.Final(hashOutput);
				
				allHashes[*(u64*)hashOutput].push_back(std::make_pair(*(block*)hashOutput, j));
			}
		}
		
		// std::cout << "Receiver hash outputs computed\n";
		timer.setTimePoint("Receiver hash outputs computed");
		
	
	
		
		///////////////// Receive hash outputs from sender and compute PSI ///////////////////

		u8* recvBuff = new u8[bucket2 * hashLengthInBytes];
		
		auto psi = 0;
		
		for (auto low = 0; low < senderSize; low += bucket2) {
			auto up = low + bucket2 < senderSize ? low + bucket2 : senderSize;
			
			ch.recv(recvBuff, (up - low) * hashLengthInBytes);
			
			for (auto idx = 0; idx < up - low; ++idx) {
				u64 mapIdx = *(u64*)(recvBuff + idx * hashLengthInBytes);
				
				auto found = allHashes.find(mapIdx);
				if (found == allHashes.end()) continue;
				
				for (auto i = 0; i < found->second.size(); ++i) {
					if (memcmp(&(found->second[i].first), recvBuff + idx * hashLengthInBytes, hashLengthInBytes) == 0) {
						++psi;
						break;
					}
				}
			}
		}
		
		if (psi == 100) {
			std::cout << "Receiver intersection computed - correct!\n";
		}
		timer.setTimePoint("Receiver intersection computed");
		
		
		std::cout << timer;
		

			
		//////////////// Output communication /////////////////
	
		u64 sentData = ch.getTotalDataSent();
		u64 recvData = ch.getTotalDataRecv();
		u64 totalData = sentData + recvData;
		
		std::cout << "Receiver sent communication: " << sentData / std::pow(2.0, 20) << " MB\n";
		std::cout << "Receiver received communication: " << recvData / std::pow(2.0, 20) << " MB\n";
		std::cout << "Receiver total communication: " << totalData / std::pow(2.0, 20) << " MB\n";
	}
		
		
	

}
