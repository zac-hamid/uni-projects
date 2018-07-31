#include <sstream>
#include <iostream>
#include <chrono>
#include "Block.hpp"

using namespace std::chrono;

std::string Block::headerToString() {
	std::ostringstream o;
	o << header.prevHash;
	o << header.UTCTimeStamp;
	o << header.nonce;
	return o.str();
}

string Block::calculateBlockHash() {
	ostringstream o;
	o << headerToString();
	o << data;
	return Hasher::hashToString(Hasher::SHA256(o.str()));
}

Block::Block() {
	header = {};
	data = "";
	header.nonce = 0;
	blockHash = calculateBlockHash();
}

Block::Block(string _data, string _prevHash) {
	header.prevHash = _prevHash;
	data = _data;
	// Gets the milliseconds since UNIX epoch (Jan 1, 1970) [C++ 11 onwards only]
	header.UTCTimeStamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	header.nonce = 0;
	blockHash = calculateBlockHash();
}

Block::Block(string _data, string _prevHash, unsigned int nonce) {
	header.prevHash = _prevHash;
	data = _data;
	// Gets the milliseconds since UNIX epoch (Jan 1, 1970) [C++ 11 onwards only]
	header.UTCTimeStamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	header.nonce = nonce;
	blockHash = calculateBlockHash();
}

std::ostream& operator<<(std::ostream &os, const Block &b) {
	os << "[\n Block hash: \t\t\"" << b.blockHash << 
		"\"\n Prev block hash: \t\"" << b.header.prevHash << 
		"\"\n Data: \t\t\t\"" << b.data << 
		"\"\n Timestamp: \t\t\"" << b.header.UTCTimeStamp << 
		"\"\n Nonce: \t\t\"" << b.header.nonce <<
		"\"\n]";
	return os;
}