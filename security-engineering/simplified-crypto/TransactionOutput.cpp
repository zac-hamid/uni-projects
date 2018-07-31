#include "TransactionOutput.hpp"

TransactionOutput::TransactionOutput(std::string recipient_, double amount_, std::string parentTID_) {
	recipient = recipient_;
	amount = amount_;
	parentTID = parentTID_;
	std::ostringstream o;
	o << recipient.getKey();
	o << amount;
	o << parentTID;
	ID = Hasher::hashToString(Hasher::SHA256(o.str()));
}
