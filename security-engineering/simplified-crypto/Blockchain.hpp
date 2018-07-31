#ifndef BLOCKCHAIN_HPP
#define BLOCKCHAIN_HPP

#include <vector>
#include <map>
#include "Block.hpp"
#include "TransactionOutput.hpp"

class Blockchain {
public:
	Blockchain();
	Blockchain(Block genesisBlock);
	bool isValid(); //Checks if blockchain is valid
	std::vector<Block>& getBlockChain();
	bool mineNewBlock(std::string _data, int nonce);

	std::map<std::string, TransactionOutput> getUTXOs() { return UTXOs; }

	void putUTXO(std::string s, TransactionOutput o) {
		UTXOs.insert(std::pair<std::string, TransactionOutput>(s, o));
	}

	void removeUTXO(std::string s) {
		UTXOs.erase(s);
	}
private:
	const unsigned int difficulty = 1;
	std::map<std::string, TransactionOutput> UTXOs;
	std::vector<Block> blockChain;
	friend std::ostream& operator<<(std::ostream& o, const Blockchain& bc);
};

#endif