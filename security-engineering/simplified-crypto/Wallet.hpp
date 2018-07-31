#ifndef WALLET_HPP
#define WALLET_HPP

#include <map>
#include "EllipticCurve.hpp"
#include "InfInt.h"
#include "PublicKey.hpp"
#include "Transaction.hpp"
#include "TransactionOutput.hpp"

class Wallet {
	InfInt privateKey;
	PublicKey publicKey;
	void generateKeyPair(EllipticCurve ec);
	EllipticCurve ec;
	std::map<std::string, TransactionOutput> myUTXOs;
public:
	Wallet(EllipticCurve ec_);
	std::string getPublicKey();
	InfInt getPrivateKey();

	double getBalance();

	Transaction sendFunds(std::string to, double amount);

};

#endif