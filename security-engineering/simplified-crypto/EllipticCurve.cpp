#include "EllipticCurve.hpp"


bool debug_flag = false;
/*
	brainpoolP384t1 parameters:
	a	= -3
	b	= 19596161053329239268181228455226581162286252326261019516900162717091837027531392576647644262320816848087868142547438
	p	= 21659270770119316173069236842332604979796116387017648600081618503821089934025961822236561982844534088440708417973331

	gx	= 3827769047710394604076870463731979903132904572714069494181204655675960538951736634566672590576020545838501853661388
	gy	= 5797643717699939326787282953388004860198302425468870641753455602553471777319089854136002629714659021021358409132328
	n	= 21659270770119316173069236842332604979796116387017648600075645274821611501358515537962695117368903252229601718723941

	l = 25

	l is the solution to RANDOM_MAX^l < n, which I use to find how many times I can multiply rand() and have it be < n
	So l < log_[RANDOM_MAX] (n)

	secp256k1 parameters:
	a = 0
	b = 7
	p = 115792089237316195423570985008687907853269984665640564039457584007908834671663

	gx = 55066263022277343669578718895168534326250603453777594175500187360389116729240
	gy = 32670510020758816978083085130507043184471273380659243275938904335757337482424
	n = 115792089237316195423570985008687907852837564279074904382605163141518161494337

	l = 17
*/
Point::Point() {
	x = InfInt(0);
	y = InfInt(0);
}

Point::Point(std::string x_, std::string y_) {
	x = InfInt(x_);
	y = InfInt(y_);
}

InfInt Point::getX() {
	return x;
}

InfInt Point::getY() {
	return y;
}

bool Point::operator==(const Point & rhs) const
{
	if (x == rhs.x && y == rhs.y) return true;
	else return false;
}

EllipticCurve::EllipticCurve() {
	a = InfInt(0);
	b = InfInt(0);
	p = InfInt(0);
	generator = Point("0", "0");
	n = InfInt(0);
	l = 0;
}

EllipticCurve::EllipticCurve(std::string a_, std::string b_, std::string p_, std::string gx, std::string gy, std::string n_, int l_) {
	a = InfInt(a_);
	b = InfInt(b_);
	p = InfInt(p_);
	generator = Point(gx, gy);
	n = InfInt(n_);
	l = l_;
}

EllipticCurve::EllipticCurve(CurveType c)
{
	switch (c) {
	case brainpoolP384t1:
		a = InfInt("-3");
		b = InfInt("19596161053329239268181228455226581162286252326261019516900162717091837027531392576647644262320816848087868142547438");
		p = InfInt("21659270770119316173069236842332604979796116387017648600081618503821089934025961822236561982844534088440708417973331");
		generator = Point("3827769047710394604076870463731979903132904572714069494181204655675960538951736634566672590576020545838501853661388", 
						"5797643717699939326787282953388004860198302425468870641753455602553471777319089854136002629714659021021358409132328");
		n = InfInt("21659270770119316173069236842332604979796116387017648600075645274821611501358515537962695117368903252229601718723941");
		l = 25;
		break;
	case secp256k1:
		a = InfInt("0");
		b = InfInt("7");
		p = InfInt("115792089237316195423570985008687907853269984665640564039457584007908834671663");
		generator = Point("55066263022277343669578718895168534326250603453777594175500187360389116729240", 
						"32670510020758816978083085130507043184471273380659243275938904335757337482424");
		n = InfInt("115792089237316195423570985008687907852837564279074904382605163141518161494337");
		l = 17;
		break;
	default:
		break;
	}
}

/*
	A function for taking the mod of a large number quickly
*/
InfInt EllipticCurve::mod(InfInt i, InfInt m) {
	InfInt div = i / m;

	if (div < 0) {
		div--;
		div *= -1;
	} else if (div == 0) {
		// i is smaller than p, can still be negative though
		if (i < 0) {
			div = 1;
		}
	} else {
		div *= -1;
	}
	
	i += (div * m);
	return i;
}

/*
	Function for adding two integers over the field F_p
*/
InfInt EllipticCurve::add(InfInt i, InfInt j)
{
	InfInt added = i + j;
	return mod(added, p);
}

/*
	Function for subtracting two integers over the field F_p
*/
InfInt EllipticCurve::sub(InfInt i, InfInt j)
{
	InfInt subbed = i - j;
	return mod(subbed,p);
}

/*
	Function for multiplying two integers over the field F_p
*/
InfInt EllipticCurve::multiply(InfInt i, InfInt j)
{
	InfInt mul = i * j;
	return mod(mul,p);
}

/*
	Function for dividing two integers over the field F_p using the Extended Euclidean Algorithm
*/
InfInt EllipticCurve::divide(InfInt i, InfInt j, InfInt m)
{
	return mod((i * EEA(m, j, m)), m);
}

/*
	An implementation of the Extended Euclidean Algorithm
	Restructured from python code given at https://www.johannes-bauer.com/compsci/ecc/#anchor07
*/
InfInt EllipticCurve::EEA(InfInt i, InfInt j, InfInt modulo) {
	InfInt s = InfInt(1), t = InfInt(0), u = InfInt(0), v = InfInt(1);
	while (j != 0) {
		InfInt q = i / j;
		if (q < 0) q--; // Need to mimic python's // operator
		InfInt r = i % j;

		InfInt unew = s;
		InfInt vnew = t;

		s = u - (q * s);
		t = v - (q * t);

		i = j;
		j = r;
		u = unew;
		v = vnew;
	}
	InfInt m = u;
	// Normally you return d, m and n
	// Only need m though
	if (modulo == 0) return m;
	else return mod (m, modulo);
}

/*
	Function to add 2 points together on an Elliptic Curve
*/
Point EllipticCurve::addPoints(Point P, Point Q)
{
	if ((P.getX() == Q.getX()) && (P.getY() == Q.getY())) return doublePoint(P);

	InfInt s = divide((sub(P.getY(), Q.getY())), (sub(P.getX(), Q.getX())), p);

	if ((P.getX() == Q.getX()) || (P.getY() == -Q.getY())) {
		return Point("0", "0");
	}
	InfInt Rx = sub( sub( multiply(s,s), P.getX()), Q.getX());
	InfInt Ry = sub(multiply(s, sub(P.getX(), Rx)), P.getY());
		Rx = mod(Rx, p);
		Ry = mod(Ry, p);
	return Point(Rx.toString(), Ry.toString());
}

/*
	Function to add a point to itself on an Elliptic Curve
*/
Point EllipticCurve::doublePoint(Point P)
{
	InfInt s = divide(add(multiply(3, multiply( P.getX(), P.getX())), a), multiply(2, P.getY()), p);

	InfInt Rx = (s * s) - (P.getX() + P.getX());
	InfInt Ry = s * (P.getX() - Rx) - P.getY();

	// Result must be taken mod p
		Rx = mod(Rx, p);
		Ry = mod(Ry, p);
	return Point(Rx.toString(), Ry.toString());
}

Point EllipticCurve::scalarMultiply(Point P, InfInt s) {

	std::vector<InfInt> ii_vec;
	std::vector<Point> p_vec;

	InfInt x = 1;
	Point R = P;
	int i = 1;

	p_vec.push_back(R);
	ii_vec.push_back(x);

	while ((x*2) < s) {
		R = doublePoint(R);
		if (debug_flag) if (!isPointOnCurve(R)) std::cerr << "Point R(" << R.getX() << ", " << R.getY() << ") is not on the curve!" << std::endl;
		x *= 2;

		p_vec.push_back(R);
		ii_vec.push_back(x);
		i++;
	}

	int k = ii_vec.size() - 1;

	for (std::vector<InfInt>::iterator it = (ii_vec.end()); it != ii_vec.begin(); --it) {
		InfInt i = ii_vec.at(k);

		if ((x + i) < s) {
			R = addPoints(R, p_vec.at(k));
			if (debug_flag) if (!isPointOnCurve(R)) std::cerr << "Point R(" << R.getX() << ", " << R.getY() << ") is not on the curve!" << std::endl;
			x += i;

		}
		k--;
	}

	//finally may need to add 1
	if ((x + 1) == s) {
		R = addPoints(R, P);
		if (debug_flag) if (!isPointOnCurve(R)) std::cerr << "Point R(" << R.getX() << ", " << R.getY() << ") is not on the curve!" << std::endl;
		x++;
	}
	return R;
}

/*
	Checks if a point satisfies the curve equation y^2 = x^3 + ax + b
*/
bool EllipticCurve::isPointOnCurve(Point P)
{
	InfInt py_sq = multiply(P.getY(), P.getY());

	InfInt px_cu = multiply(P.getX(), multiply(P.getX(), P.getX()));

	InfInt a_px = multiply(a, P.getX());

	//Result must be taken mod p
	return (mod(py_sq,p) == mod(px_cu + a_px + b,p));
}

/*
	Generates a private-public key pair
*/
EllipticCurve::KeyPair EllipticCurve::generateKeyPair(bool verbose)
{
	if (verbose) std::clog << "Generating private key..." << std::endl;

	InfInt privateKey = fetchRandNum();
	
	if (verbose) std::clog << "Private key: " << privateKey << std::endl;

	if (verbose) std::clog << "Generating public key, this may take some time..." << std::endl;
	Point publicKey = scalarMultiply(generator, privateKey);
	if (verbose) std::clog << "Public key: (" << publicKey.getX() << ", " << publicKey.getY() << ")" << std::endl;
	KeyPair kp = KeyPair();
	kp.privateKey = privateKey;
	kp.publicKey = publicKey;

	return kp;
}

InfInt EllipticCurve::fetchRandNum() {
	InfInt randNum(0);

	// If l = 0, we can assume n < RANDOM_MAX
	if (l == 0) {
		srand(time(NULL) + rand());
		randNum = rand() % n.toInt();
		while (randNum == 0) randNum = rand() % n.toInt();
	} else {
		// l won't be very large at all (since it is a sol'n to RANDOM_MAX^l < n)
		// Will map range of rand(): [0, RAND_MAX] -> [lowerKeygenRange, l]
		// Using a lower bound of around half of l helps to guarantee that large private keys are generated

		int lowerKeygenRange = l / 2;

		// Range is now [0, l]
		int x = rand() % l;

		// Range is now [lowerKeygenRange, l]
		x = lowerKeygenRange + ((l - lowerKeygenRange) / l) * x;
		randNum = 1;

		// This is not the most secure way of generating a private key,
		// it is for demonstration purposes ONLY (it is fast)
		for (int i = 0; i < x; i++) {
			srand(time(NULL));
			// Ensures rand() will be at maximum RANDOM_MAX = 32767
			// This prevents system-dependent behaviour of rand()
			randNum *= (rand() % RANDOM_MAX);
		}
	}
	return randNum;
}

EllipticCurve::Signature EllipticCurve::sign(std::string message, InfInt privateKey) {
	// Given a message, hashes it with SHA256 -> Converts to decimal -> Stores as InfInt
	InfInt e(hexToKey(Hasher::hashToString(Hasher::SHA256(message))));
	
	InfInt s_(0);
	InfInt r_(0);
	
	while (s_ == 0) {
		InfInt k = fetchRandNum();

		Point R = scalarMultiply(generator, k);
		r_ = mod(R.getX(), n);
		
		while (r_ == 0) {
			k = fetchRandNum();
			R = scalarMultiply(generator, k);
			r_ = mod(R.getX(), n);
		}

		InfInt numerator = mod(e + (privateKey * r_), n);
		s_ = divide(numerator, k, n);
	}
	Signature s = Signature();
	s.r = r_;
	s.s = s_;
	return s;
}

bool EllipticCurve::isSigVerified(std::string message, EllipticCurve::Signature s, Point publicKey) {
	// Given a message, hashes it with SHA256 -> Converts to decimal -> Stores as InfInt
	InfInt e(hexToKey(Hasher::hashToString(Hasher::SHA256(message))));
	
	InfInt w = divide(1, s.s, n);

	InfInt u1 = mod(e * w, n);
	InfInt u2 = mod(s.r * w, n);

	Point Q1 = scalarMultiply(generator, u1);
	Point Q2 = scalarMultiply(publicKey, u2);

	Point P = addPoints(Q1, Q2);
	
	return (mod(P.getX(),n) == mod(s.r, n));
}

Point EllipticCurve::getGenerator()
{
	return generator;
}

std::string EllipticCurve::keyToHex(InfInt key) {
	const BaseConverter& dec2hex = BaseConverter::DecimalToHexConverter();
	return dec2hex.Convert(key.toString());
}

std::string EllipticCurve::hexToKey(std::string hex_string) {
	const BaseConverter& hex2dec = BaseConverter::HexToDecimalConverter();
	return hex2dec.Convert(hex_string);
}