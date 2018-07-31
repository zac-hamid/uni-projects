#include "Blockchain.hpp"
#include <iostream>

Blockchain::Blockchain() { }

Blockchain::Blockchain(Block genesisBlock) {
	// Need to do some checks here?
	blockChain.push_back(genesisBlock);
}

bool Blockchain::isValid() {
	std::string targetHash;
	for (int i = 0; i < difficulty; i++) {
		targetHash.append("0");
	}
	for (int i = 0; i < blockChain.size(); i++) {
		// Current block hash not equal to calculated current block hash
		if (blockChain.at(i).blockHash != blockChain.at(i).calculateBlockHash()) {
			return false;
		}
		if (i > 0) {
			// Previous block hash not equal to current block's prevHash
			if (blockChain.at(i-1).blockHash != blockChain.at(i).header.prevHash) {
				return false;
			}
		}
		if (blockChain.at(i).blockHash.substr(0, difficulty) != targetHash) return false;
	}
	return true;
}

// This is purely for testing if the isValid() method detects changes done by an outsider to data in the blocks of the chain
std::vector<Block>& Blockchain::getBlockChain() {
	return blockChain;
}

bool Blockchain::mineNewBlock(std::string _data, int nonce) {
	Block newBlock;
	if (blockChain.size() > 0) newBlock = Block(_data, blockChain.at(blockChain.size()-1).blockHash, nonce);
	else newBlock = Block(_data, "0", nonce);

	std::string targetHash;
	for (int i = 0; i < difficulty; i++) {
		targetHash.append("0");
	}

	if (newBlock.calculateBlockHash().substr(0, difficulty) == targetHash) {
		if (isValid()) {
			// Block was mined successfully, accept block into blockchain
			std::cout << "Mined block: \n" << newBlock << endl;
			blockChain.push_back(newBlock);
			return true;
		}
	}
	return false;
}

std::ostream& operator<<(std::ostream &o, const Blockchain &bc) {
	o << "{\n";
	for (Block b : bc.blockChain) {
		o << b << endl;
	}
	o << "}";
	return o;
}
