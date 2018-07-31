#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <stdint.h>
#include <time.h>
#include <string>
#include "Hasher.hpp"

using namespace std;

class Block {
private:
    struct BlockHeader {
		string prevHash;			// Hash of previous block header (256-bit, might change)
		//uint32_t merkleRoot;		// Hash based on all of the transactions in the block
		long long UTCTimeStamp;		// Current timestamp in milliseconds (UTC) since epoch
		//int bits;
		unsigned int nonce;
    };

	string headerToString();

public:
	BlockHeader header;
	string data;
	string blockHash;

	Block();
    Block(string _data, string _prevHash);
	Block(string _data, string _prevHash, unsigned int nonce);
	string calculateBlockHash();

	friend std::ostream& operator<<(std::ostream &os, const Block &b);
};

#endif