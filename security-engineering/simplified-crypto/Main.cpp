#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include "BaseConverter.hpp"
#include "Hasher.hpp"
#include "Blockchain.hpp"
#include "EllipticCurve.hpp"
#include "InfInt.h"
#include "Wallet.hpp"
#include "Transaction.hpp"

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]) {
	/*Blockchain bc;
	unsigned int nonce = 0;
	string data;
	long long prevTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	const int BLOCKS_TO_MINE = 5;
	int times[BLOCKS_TO_MINE];
	int totalTimeToMineBlockchain = 0;

	for (int i = 0; i < BLOCKS_TO_MINE; i++) {
		if (i == 0) data = "Genesis block.";
		else data = "This is block " + to_string(i) + ".";
		while (!bc.mineNewBlock(data, nonce)) {
			nonce++;
		}
		long long time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		cout << "It took " << (time - prevTime) / 1000 << " seconds to mine." << endl;
		times[i] = (time - prevTime) / 1000;
		totalTimeToMineBlockchain += times[i];
		nonce = 0;
		prevTime = time;
	}
	cout << "Successfully mined " << bc.getBlockChain().size() << " blocks!" << endl;

	int avgTime = 0;
	for (int i = 0; i < BLOCKS_TO_MINE; i++) {
		avgTime += times[i];
	}
	avgTime /= BLOCKS_TO_MINE;

	cout << "Average mining time was " << avgTime << " seconds." << endl;
	cout << "Total time to mine was " << totalTimeToMineBlockchain << " seconds." << endl;
	

	cout << "---------------------" << endl;
	cout << "\t\tNew section\t\t" << endl;*/
	cout << "Transaction signing and verifying demonstration using wallets" << endl;
	cout << "Using brainpoolP384t1 curve" << endl;
	EllipticCurve ec(EllipticCurve::CurveType::brainpoolP384t1);
	cout << "Creating wallet A..." << endl;
	Wallet wA(ec);
	cout << "Creating wallet B..." << endl;
	Wallet wB(ec);
	cout << "Wallet A address: " << wA.getPublicKey() << endl;
	cout << "Wallet B address: " << wB.getPublicKey() << endl;
	std::vector<TransactionInput> inputs;
	Transaction t = Transaction(ec, wA.getPublicKey(), wA.getPublicKey(), 5.0, inputs);
	t.genSignature(wA.getPrivateKey().toString());
	cout << "Checking signature..." << endl;
	if (t.isSignatureValid()) cout << "Signature is valid!" << endl;
	else cout << "Signature is not valid..." << endl;

	//cout << "Elliptic-Curve Cryptography Working Demonstration" << endl;
	////cout << "Using curve y^2 = x^3 + x + 1  mod 263; G = (148, 27), n = 260" << endl;
	//cout << "--------------------------------------------------" << endl;
	////EllipticCurve ec("1", "1", "263", "224", "9", "13", 0);
	//cout << "Bob key-pair generation: " << endl;
	//EllipticCurve::KeyPair bobKp = ec.generateKeyPair(true);
	//cout << "---------------------------------" << endl;
	//cout << "Alice key-pair generation: " << endl;
	//EllipticCurve::KeyPair aliceKp = ec.generateKeyPair(true);
	//cout << "---------------------------------" << endl;
	////string hexed = ec.keyToHex(bobKp.publicKey.getX().toString());
	////cout << "Hexed: " << hexed << endl;

	////cout << "Unhexed: " << ec.hexToKey(hexed) << endl;
	///*
	//InfInt r = ec.fetchRandNum();
	//cout << "Random num r = " << r << endl;

	//Point R = ec.scalarMultiply(ec.getGenerator(), r);
	//cout << "Alice's sent message = (" << R.getX() << ", " << R.getY() << ")" << endl;

	//Point S = ec.scalarMultiply(bobKp.publicKey, r);
	//cout << "Alice's Secret = (" << S.getX() << ", " << S.getY() << ")" << endl;

	//S = ec.scalarMultiply(R, bobKp.privateKey);
	//cout << "Bob's Calculated Secret = (" << S.getX() << ", " << S.getY() << ")" << endl;
	//*/
	//
	//string message = "Hello world!";
	//cout << "Bob will sign the message: " << message << endl;
	//EllipticCurve::Signature s = ec.sign(message, bobKp.privateKey);

	//cout << "Signature = (" << s.r << ", " << s.s << ")" << endl;

	//cout << "--------------------------------" << endl;
	//cout << "Alice is verifying the signature..." << endl;
	//bool v = ec.isSigVerified(message, s, bobKp.publicKey);
	//if (v) cout << "Signature was verified!" << endl;
	//else cout << "Signature could not be verified..." << endl;
	///*
	//cout << "-------------------------------" << endl;
	////ec = EllipticCurve("2", "3", "97", "12", "3", "50", 0);
	//cout << "Scalar multiply test" << endl;
	//for (int i = 1; i < 13; i++) {
	//	Point P = ec.scalarMultiply(ec.getGenerator(), i);
	//	cout << "scalar multiple " << i << "G = (" << P.getX() << ", " << P.getY() << ")" << endl;
	//}*/
	int x;
	cin >> x;
	return 0;
}