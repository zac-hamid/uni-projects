#include <sys/io.h>
#include <iostream>
#include "MSIP404.h"
#include "IOPermException.h"

MSIP404::MSIP404(int baseAddress, int quadBaseAddress){
	this->baseAddress = baseAddress;
	this->quadBaseAddress = quadBaseAddress;
	try {
		if (ioperm(baseAddress,MSIP404_BYTE_SIZE,1) != 0 || ioperm(quadBaseAddress,MSIP404_QUADRATURE_BYTE_SIZE,1) != 0){
			IOPermException e;
			throw e;
		}
	} catch (exception& e) {
		cout << e.what();
	}
}

/*
	Resets the encoder on the selected channel.
	There are 8 possible channels [0,7] to reset.
*/
void MSIP404::EncoderReset(int channel) {
	if (channel < MSIP404_NUM_RESET_CHANNELS && channel >= 0) {
		outb(0x00, baseAddress + channel);
	} else {
		std::cout << "MSIP404->EncoderReset: channel out of range [0,7]!" << std::endl;
	}
}

/*
	Reads the encoder value on the selected channel.
	There are 4 possible channels [0,3] to read.
*/
long MSIP404::EncoderRead(int channel) {
	if (channel < 0 || channel >= MSIP404_NUM_ENCODER_CHANNELS) { 
		std::cout << "MSIP404->EncoderRead: channel out of range [0,3]!" << std::endl;
		return 0;
	}

	enc.b[3] = inb(baseAddress + 0x03 + 4 * channel);
	enc.b[2] = inb(baseAddress + 0x02 + 4 * channel);
	enc.b[1] = inb(baseAddress + 0x01 + 4 * channel);
	enc.b[0] = inb(baseAddress + 4 * channel);

	return (long)enc.a;
}

int MSIP404::GetBaseAddress() {
	return baseAddress;
}

int MSIP404::GetQuadBaseAddress() {
	return quadBaseAddress;
}

bool MSIP404::DetectIndexPulse(unsigned char channel){
	switch(channel){
		case 0:
			return ((inb(quadBaseAddress) & 0x80) != 0);
		case 1:
			return ((inb(quadBaseAddress) & 0x20) != 0);
		case 2:
			return ((inb(quadBaseAddress + 0x01) & 0x80) != 0);
		default:
			std::cout << "MSIP404->DetectPositiveIndexPulse: channel out of range [0,2]!" << std::endl;
			return 0;
	}
}

void MSIP404::operator!() {
	for (int i = 0; i < 8; i++) {	//i = channel to reset
		EncoderReset(i);
	}
}
