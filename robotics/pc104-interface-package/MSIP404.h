#ifndef MSIP404_H
#define MSIP404_H

#define MSIP404_BYTE_SIZE 16
#define MSIP404_QUADRATURE_BYTE_SIZE 2

#define MSIP404_NUM_RESET_CHANNELS 8
#define MSIP404_NUM_ENCODER_CHANNELS 4

#define MSIP404_CHANNEL_0 0
#define MSIP404_CHANNEL_1 1
#define MSIP404_CHANNEL_2 2
#define MSIP404_CHANNEL_3 3
#define MSIP404_CHANNEL_4 4
#define MSIP404_CHANNEL_5 5
#define MSIP404_CHANNEL_6 6
#define MSIP404_CHANNEL_7 7

#define MSIP404_QUADRATURE_CHANNEL_0 0
#define MSIP404_QUADRATURE_CHANNEL_1 1
#define MSIP404_QUADRATURE_CHANNEL_2 2

union encode {
	int a;
	char b[4];
};

class MSIP404 {
private:
	encode enc;
	int baseAddress;
	int quadBaseAddress;
public:
	void EncoderReset(int channel);
	long EncoderRead(int channel);
	int GetBaseAddress();
	int GetQuadBaseAddress();
	bool DetectIndexPulse(unsigned char channel);
	void operator!(void);
	MSIP404(int baseAddress, int quadBaseAddress);
};

#endif
