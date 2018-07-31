#include <stdint.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "Hasher.hpp"

namespace Hasher {
	int64byte Hasher::SHA256(std::string data) {

		int dataSize = data.size();

		// First 32 bits of the fractional part of the square roots of the first 8 primes
		uint32_t
			h0 = 0x6A09E667,
			h1 = 0xBB67AE85,
			h2 = 0x3C6EF372,
			h3 = 0xA54FF53A,
			h4 = 0x510E527F,
			h5 = 0x9B05688C,
			h6 = 0x1F83D9AB,
			h7 = 0x5BE0CD19;

		// First 32 bits of fractional part of cube roots of the first 64 primes
		int32_t k[64] = {
			0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
			0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
			0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
			0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
			0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
			0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
			0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
			0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
		};

		// Find K (how many 0 bits to append to message to pad it out)
		uint64_t dataSizeBits = dataSize * BITS_PER_BYTE;
		int K;
		for (int i = 1; i < INT_MAX; i++) {
			if ((dataSizeBits + 1 + 64 + i) % 512 == 0) {
				K = i;
				break;
			}
		}

		// Allocate memory and clear it for required number of 512-bit chunks
		BYTE *ptr = (BYTE*)calloc((dataSizeBits + 1 + K + 64) / BITS_PER_BYTE, sizeof(BYTE));
		memcpy(ptr, data.c_str(), dataSize);								// Copy initial message
		BYTE *tempPtr = ptr;
		tempPtr += dataSize;
		*tempPtr |= (0x80);													// Append padding (a single 1 bit)

		tempPtr += (K + 1) / BITS_PER_BYTE;									// Append K 0's (already 0's because we used calloc()) K will always be odd

		uint64_t dataSizeBitsTemp = dataSizeBits;
		if (!isBigEndian()) swapLong(&dataSizeBitsTemp);					// If system is little-endian we need to swap the order of the bytes in dataSizeBits
		memcpy(tempPtr, &dataSizeBitsTemp, sizeof(uint64_t));				// Append original message length as big-endian 64-bit integer

		// Used to break message into 512-bit chunks

		BYTE *chunk;

		for (int j = 0; j < (dataSizeBits + 1 + K + 64) / 512; j++) {
			chunk = ptr;

			// Perform the following on each chunk
			WORD w[PAGE_SIZE];
			memcpy(w, chunk, CHUNK_SIZE);			// Copy first 512-bit chunk into the word array

			// Need to swap words if in big endian
			if (!isBigEndian()) {
				for (int i = 0; i < 16; i++) swapWord(&w[i]);
			}

			// From pseudocode, perform bit operations to find w[k] for k >= 16 && k <= 63
			for (int k = 16; k < PAGE_SIZE; k++) {
				uint32_t s0 = rotr(w[k - 15], 7) ^ rotr(w[k - 15], 18) ^ (w[k - 15] >> 3);
				uint32_t s1 = rotr(w[k - 2], 17) ^ rotr(w[k - 2], 19) ^ (w[k - 2] >> 10);
				w[k] = w[k - 16] + s0 + w[k - 7] + s1;
			}

			uint32_t
				a = h0,
				b = h1,
				c = h2,
				d = h3,
				e = h4,
				f = h5,
				g = h6,
				h = h7;

			// From pseudocode, perform bit manipulations to get current message digest (for up to this 512-bit chunk)
			for (int n = 0; n < PAGE_SIZE; n++) {
				uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
				uint32_t ch = (e & f) ^ ((~e) & g);
				uint32_t temp1 = h + S1 + ch + k[n] + w[n];
				uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
				uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
				uint32_t temp2 = S0 + maj;

				h = g;
				g = f;
				f = e;
				e = d + temp1;
				d = c;
				c = b;
				b = a;
				a = temp1 + temp2;

			}

			h0 = h0 + a;
			h1 = h1 + b;
			h2 = h2 + c;
			h3 = h3 + d;
			h4 = h4 + e;
			h5 = h5 + f;
			h6 = h6 + g;
			h7 = h7 + h;

			//Go to next 512-bit chunk
			ptr += CHUNK_SIZE;
		}

		// Once all chunks visited, get final message digest (hashed string)
		int64byte hashResult;

		hashResult.i[0] = h0;
		hashResult.i[1] = h1;
		hashResult.i[2] = h2;
		hashResult.i[3] = h3;
		hashResult.i[4] = h4;
		hashResult.i[5] = h5;
		hashResult.i[6] = h6;
		hashResult.i[7] = h7;

		return hashResult;
	}

	// Circular right bit shift; n = 4-byte number to shift, c = shift amount (in bits)
	uint32_t Hasher::rotr(uint32_t n, int c) {
		return (n >> c) | (n << ((sizeof(uint32_t) * BITS_PER_BYTE) - c));
	}

	// Circular left bit shift; n = 4-byte number to shift, c = shift amount (in bits)
	uint32_t Hasher::rotl(uint32_t n, int c) {
		return (n << c) | (n >> ((sizeof(uint32_t) * BITS_PER_BYTE) - c));
	}

	std::string hashToString(int64byte hash)
	{
		std::ostringstream o;
		for (int i = 0; i < (sizeof(hash.i) / sizeof(uint64_t)); i++) {
			// Sends each 8-byte part of the hash to the ostringstream, ensuring any leading zeros are preserved
			o << std::hex << std::setfill('0') << std::setw(8) << hash.i[i];
		}
		return o.str();
	}

	// Reverses order of bytes in an 8 byte number; x = address of number to swap
	void Hasher::swapLong(uint64_t *x) {
		*x = (*x & 0x00000000FFFFFFFF) << 32 | (*x & 0xFFFFFFFF00000000) >> 32;
		*x = (*x & 0x0000FFFF0000FFFF) << 16 | (*x & 0xFFFF0000FFFF0000) >> 16;
		*x = (*x & 0x00FF00FF00FF00FF) << 8 | (*x & 0xFF00FF00FF00FF00) >> 8;
	}

	// Reverses order of bytes in a 4 byte number; word = address of number to swap
	void Hasher::swapWord(WORD *word) {
		WORD w = *word;
		char bytes[4];
		bytes[0] = (w >> 24) & 0xFF;
		bytes[1] = (w >> 16) & 0xFF;
		bytes[2] = (w >> 8) & 0xFF;
		bytes[3] = w & 0xFF;
		memcpy(word, bytes, 4);
	}

	// Quick check to see if a system is big-endian
	bool Hasher::isBigEndian(void)
	{
		union {
			uint32_t i;
			char c[4];
		} bint = { 0x01020304 };

		return bint.c[0] == 1;
	}
}