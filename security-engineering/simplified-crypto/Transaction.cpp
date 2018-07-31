#include "Transaction.hpp"
#include "Blockchain.hpp"

std::string Transaction::calculateHash() {
	numTransactions++;
	std::ostringstream o;
	o << sender.getKey();
	o << recipient.getKey();
	o << amount;
	o << numTransactions;
	return Hasher::hashToString(Hasher::SHA256(o.str()));
}

/*
	Assumes from and to addresses are passed in as a space-delimited HEX address
	First half is the x co-ordinate of public key, second half is y co-ordinate
*/
Transaction::Transaction(EllipticCurve ec_, std::string from, std::string to, double amount_, std::vector<TransactionInput> inputs_) {
	ec = ec_;
	sender = PublicKey(from);
	recipient = PublicKey(to);
	amount = amount_;
	inputs = inputs_;
}

// Signs all the important data that shouldn't be tampered with

void Transaction::genSignature(std::string privateKey) {
	std::ostringstream o;
	o << sender.getKey();
	o << recipient.getKey();
	o << amount;
	sig = ec.sign(o.str(), InfInt(privateKey));
}

bool Transaction::isSignatureValid() {
	std::ostringstream o;
	o << sender.getKey();
	o << recipient.getKey();
	o << amount;
	return ec.isSigVerified(o.str(),sig,sender.getPointKey());
}

bool Transaction::processNewTransaction(Blockchain& bc) {

	if (!isSignatureValid()) {
		std::cerr << "Failed to verify transaction signature!" << std::endl;
		return false;
	}

	for (std::vector<TransactionInput>::iterator it = inputs.begin(); it != inputs.end(); ++it) {
		it->setUTXO(bc.getUTXOs().find(it->getTOutID())->second);
	}

	double remaining = getValueOfInputs(bc) - amount;
	transactionID = calculateHash();
	outputs.push_back(TransactionOutput(recipient.getKey(), amount, transactionID));
	outputs.push_back(TransactionOutput(sender.getKey(), remaining, transactionID));

	for (std::vector<TransactionOutput>::iterator it = outputs.begin(); it != outputs.end(); ++it) {
		bc.putUTXO(it->getID(), *it);
	}

	for (std::vector<TransactionInput>::iterator it = inputs.begin(); it != inputs.end(); ++it) {
		if (it->getUTXO() == bc.getUTXOs().end()->second) continue;
		bc.removeUTXO(it->getUTXO().getID());
	}

	return true;
}

double Transaction::getValueOfInputs(Blockchain &bc)
{
	double total = 0.0;
	for (std::vector<TransactionInput>::iterator it = inputs.begin(); it != inputs.end(); ++it) {
		if (it->getUTXO() == bc.getUTXOs().end()->second) continue;
		total += it->getUTXO().getAmount();
	}
	return total;
}

double Transaction::getValueOfOutputs()
{
	double total = 0.0;
	for (std::vector<TransactionOutput>::iterator it = outputs.begin(); it != outputs.end(); ++it) {
		total += it->getAmount();
	}
	return total;
}
