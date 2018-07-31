#include "PublicKey.hpp"
PublicKey::PublicKey() {
	key = Point("0", "0");
}
/*
	Assumes string s comes in as a HEX string with 1 space somewhere in the middle
*/
PublicKey::PublicKey(std::string s) {
	std::vector<std::string> result;
	std::istringstream iss(s);
	for (std::string s; iss >> s; )
		result.push_back(s);
	int i = 0;
	if (result.size() >= 2) {
		key = Point(EllipticCurve::hexToKey(result.at(0)), EllipticCurve::hexToKey(result.at(1)));
	}
	else if (result.size() == 1) {
		key = Point(EllipticCurve::hexToKey(result.at(0)), "0");
	}
	else { key = Point("0", "0"); }
}

/*
	Gets the public key as a space delimited hex string
*/
std::string PublicKey::getKey() {
	std::ostringstream oss;
	oss << EllipticCurve::keyToHex(key.getX());
	oss << " ";
	oss << EllipticCurve::keyToHex(key.getY());

	return oss.str();
}

bool PublicKey::operator==(const PublicKey & rhs) const {
	if (key == rhs.key) return true;
	else return false;
}
