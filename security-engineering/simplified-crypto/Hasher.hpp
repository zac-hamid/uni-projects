#ifndef HASHER_HPP
#define HASHER_HPP

#include <stdint.h>
#include <string>

#define CHUNK_SIZE 64			// 64 bytes [512-bit]
#define PAGE_SIZE 64			// Number of words in array
#define BITS_PER_BYTE 8			// Number of bits in a byte

typedef uint32_t WORD;
typedef unsigned char BYTE;

// To contain the final message digest (SHA256-hashed string)
struct int64byte {
	uint64_t i[8];
};

namespace Hasher {
	int64byte SHA256(std::string data);
	uint32_t rotr(uint32_t n, int c);
	uint32_t rotl(uint32_t n, int c);
	std::string hashToString(int64byte hash);
	void swapLong(uint64_t *x);
	void swapWord(WORD *word);
	bool isBigEndian(void);
};

#endif