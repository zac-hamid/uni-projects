#include "Wallet.hpp"

Wallet::Wallet(EllipticCurve ec_) {
	ec = ec_;
	generateKeyPair(ec_);
}

std::string Wallet::getPublicKey() {
	return publicKey.getKey();
}

InfInt Wallet::getPrivateKey() {
	return privateKey;
}

double Wallet::getBalance()
{
	double total = 0.0;
	std::map<std::string, TransactionOutput>::iterator it;
	for (it = myUTXOs.begin(); it != myUTXOs.end(); it++) {
		if (it->second.isTXOMine(publicKey)) {
			total += it->second.getAmount();
			myUTXOs.insert(std::pair<std::string, TransactionOutput>(it->second.getID(), it->second));
		}
	}
	return total;
}

Transaction Wallet::sendFunds(std::string to, double amount)
{
	if (getBalance() < amount) {
		std::cerr << "Not enough funds to perform transaction!" << std::endl;
		return Transaction(false);
	}

	std::vector<TransactionInput> inputs;

	double total = 0.0;

	std::map<std::string, TransactionOutput>::iterator it;
	for (it = myUTXOs.begin(); it != myUTXOs.end(); it++) {
		total += it->second.getAmount();
		inputs.push_back(TransactionInput(it->second.getID()));
		if (total > amount) break;
	}

	Transaction newTrans = Transaction(ec, publicKey.getKey(), to, amount, inputs);
	newTrans.genSignature(privateKey.toString());

	for (std::vector<TransactionInput>::iterator it = inputs.begin(); it != inputs.end(); it++) {
		myUTXOs.erase(it->getTOutID);
	}

	return newTrans;
}

void Wallet::generateKeyPair(EllipticCurve ec) {
	EllipticCurve::KeyPair kp = ec.generateKeyPair(false);
	privateKey = kp.privateKey;
	publicKey = PublicKey(kp.publicKey);
}
