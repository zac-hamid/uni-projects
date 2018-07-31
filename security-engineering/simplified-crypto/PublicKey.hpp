#ifndef PUBLICKEY_HPP
#define PUBLICKEY_HPP

#include <sstream>
#include "EllipticCurve.hpp"

class PublicKey {
	Point key;
public:
	PublicKey();
	PublicKey(Point k) { key = k; }
	PublicKey(std::string s);

	std::string getKey();

	Point getPointKey() { return key; }

	bool operator==(const PublicKey& rhs) const;
};

#endif