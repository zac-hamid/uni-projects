#include <sys/io.h>
#include <iostream>
#include "DAC06.h"
#include "IOPermException.h"

DAC06::DAC06(int baseAddress, unsigned char range){
	this->baseAddress = baseAddress;
	this->range = range;
	try {
		if (ioperm(baseAddress,DAC06_BYTE_SIZE,1) != 0){
			IOPermException e;
			throw e;
		}
	} catch (exception& e) {
		cout << e.what();
	}
}

int DAC06::GetBaseAddress(){
	return baseAddress;
}

void DAC06::AnalogOut(unsigned char channel, double voltage){
	bool bipolar = false;
	unsigned char totalRange;
	
	if (channel >= DAC06_NUM_CHANNELS || channel < 0){
		std::cout << "DAC06->AnalogOut: channel out of range [0,5]!"
		return;
	}

	bool invalidVoltage = false;
	switch(range) {
		case DAC06_RANGE_10V_BP:
			if (voltage < -10 || voltage > 10) invalidVoltage = true;
			totalRange = 20;
			bipolar = true;
			break;
		case DAC06_RANGE_5V_BP:
			if (voltage < -5 || voltage > 5) invalidVoltage = true;
			totalRange = 10;
			bipolar = true;
			break;
		case DAC06_RANGE_10V_UP:
			if (voltage < 0 || voltage > 10) invalidVoltage = true;
			totalRange = 10;
			break;
		case DAC06_RANGE_5V_UP:
			if (voltage < 0 || voltage > 5) invalidVoltage = true;
			totalRange = 5;
			break;
		default:
			std::cout << "DAC06->AnalogOut: no valid voltage range set!" << std::endl;
			return;
			break;
	}

	if (invalidVoltage){
		std::cout << "DAC06->AnalogOut: voltage out of set range!" << std::endl;
		return;
	}

	// Output code formula changes if bipolar
	int outputCode = (int)((voltage + (bipolar*0.5*totalRange))/totalRange * 4096);

	unsigned char byte, nibble;

	outputCode &= 0xFFF;	//Make sure it is just 12 bits

	byte = outputCode & 0x0FF;
	nibble = (outputCode & 0xF00) >> 8;
	nibble &= 0x0F;

	outb(byte, baseAddress + 2 * channel);
	outb(nibble, baseAddress + 1 + 2 * channel);
}


void DAC06::setDACRange(unsigned char range){
	this->range = range;
}
