#include <iostream>
#include <exception>

using namespace std;

class IOPermException : public exception {
public:
	const char * what() const throw()
	{
		return "Unable to get I/O permissions!\n";
	}
};
