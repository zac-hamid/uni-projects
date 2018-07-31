#ifndef ELLIPTICCURVE_HPP
#define ELLIPTICCURVE_HPP

#include "Hasher.hpp"
#include "InfInt.h"
#include "BaseConverter.hpp"
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>

#define RANDOM_MAX 32767

class Point {
	InfInt x, y;
public:
	Point();
	Point(std::string x_, std::string y_);

	InfInt getX();
	InfInt getY();

	bool operator==(const Point& rhs) const;
};

class EllipticCurve {
public:
	enum CurveType {
		brainpoolP384t1,
		secp256k1
	};

	struct KeyPair {
		InfInt privateKey;
		Point publicKey;
	};

	struct Signature {
		InfInt r, s;
	};
	EllipticCurve();
	EllipticCurve(std::string a_, std::string b_, std::string p_, std::string gx, std::string gy, std::string n_, int l_);
	EllipticCurve(CurveType c);

	InfInt mod(InfInt i, InfInt m);

	InfInt add(InfInt i, InfInt j);
	InfInt sub(InfInt i, InfInt j);

	InfInt multiply(InfInt i, InfInt j);
	InfInt divide(InfInt i, InfInt j, InfInt m);

	InfInt EEA(InfInt i, InfInt j, InfInt modulo);

	Point addPoints(Point P, Point Q);
	Point doublePoint(Point P);
	Point scalarMultiply(Point P, InfInt s);

	bool isPointOnCurve(Point P);

	KeyPair generateKeyPair(bool verbose);

	InfInt fetchRandNum();

	Signature sign(std::string message, InfInt privateKey);

	bool isSigVerified(std::string message, EllipticCurve::Signature s, Point publicKey);

	Point getGenerator();
	static std::string keyToHex(InfInt key);
	static std::string hexToKey(std::string hex_string);

	InfInt getP() { return p; }
private:
	InfInt a, b, p, n;
	int l;
	Point generator;
};
#endif