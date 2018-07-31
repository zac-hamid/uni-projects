#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <string>
#include <vector>
#include <sstream>
#include "Blockchain.hpp"
#include "EllipticCurve.hpp"
#include "PublicKey.hpp"
#include "TransactionInput.hpp"
#include "TransactionOutput.hpp"

class Transaction {
	std::string transactionID;
	PublicKey sender, recipient;
	double amount;
	std::vector<TransactionInput> inputs;
	std::vector<TransactionOutput> outputs;
	EllipticCurve ec;

	EllipticCurve::Signature sig;

	InfInt numTransactions;
	bool valid; // Hidden var for invalid transactions
	std::string calculateHash();
public:
	Transaction(bool valid_) { valid = valid_; }
	Transaction(EllipticCurve ec_, std::string from, std::string to, double amount_, std::vector<TransactionInput> inputs_);

	void genSignature(std::string privateKey);

	bool isSignatureValid();

	bool processNewTransaction(Blockchain& bc);

	double getValueOfInputs(Blockchain &bc);
	double getValueOfOutputs();
};

#endif