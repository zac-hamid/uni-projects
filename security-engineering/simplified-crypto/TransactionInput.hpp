#ifndef TRANSACTIONINPUT_HPP
#define TRANSACTIONINPUT_HPP

#include "TransactionOutput.hpp"

class TransactionInput {
	std::string tOutID;
	TransactionOutput UTXO;

public:
	TransactionInput(std::string tOutID_) { tOutID = tOutID_; }

	TransactionOutput getUTXO() { return UTXO; }
	void setUTXO(TransactionOutput UTXO_) { UTXO = UTXO_; }
	std::string getTOutID() { return tOutID; }
};

#endif