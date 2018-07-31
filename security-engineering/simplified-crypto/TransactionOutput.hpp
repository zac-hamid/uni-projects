#ifndef TRANSACTIONOUTPUT_HPP
#define TRANSACTIONOUTPUT_HPP

#include "PublicKey.hpp"

class TransactionOutput {
	std::string ID;
	PublicKey recipient;
	double amount;
	std::string parentTID;

public:
	TransactionOutput() { ID = "0"; amount = 0.0; parentTID = "0"; }
	TransactionOutput(std::string recipient_, double amount_, std::string parentTID_);

	bool isTXOMine(PublicKey pk) { return (pk == recipient); }

	double getAmount() { return amount; }

	std::string getID() { return ID; }

	bool operator==(const TransactionOutput & rhs) const {
		if ((ID == rhs.ID) && (recipient == rhs.recipient) && (amount == rhs.amount) && (parentTID == rhs.parentTID)) return true;
		else return false;
	}
};

#endif