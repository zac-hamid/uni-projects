#include "PCM3718.h"
#include "IOPermException.h"
#include <iostream>
#include <sys/io.h>
#include <stdlib.h>

PCM3718::PCM3718(int baseAddress) {
	this->baseAddress = baseAddress;
	try {
		if (ioperm(baseAddress,PCM3718_BYTE_SIZE,1) != 0){
			IOPermException e;
			throw e;
		}
	} catch (exception& e) {
		cout << e.what();
	}

	// Initialise LEDs to be 0x00 so the low/high bytes we're storing are the same as what's on the board
	digOutLowByte = 0x00;
	digOutHighByte = 0x00;
	outb(digOutLowByte, baseAddress + PCM3718_DIG_OUT_LOW);
	outb(digOutHighByte, baseAddress + PCM3718_DIG_OUT_HIGH);
	rangeSelect = PCM3718_ADC_BP_5V;	// Default range is +/-5V
}

/*
	Writes a bit out to the specified address relative to the base address (specified in the header file)
	Inputs:
		- address: address relative to base (in hex)
		- bitNumber: the specific bit in the byte to write to (from 1-8 [inclusive])
		- bit: the bit to write
*/
void PCM3718::WriteBit(int address, int bitNumber, bool bit) {
	if (address < 0 || address >= PCM3718_BYTE_SIZE){
		std::cout << "PCM3718->WriteBit: address is out of range!" << std::endl;
		return;
	}

	if (bitNumber < 1 || bitNumber > 8){
		std::cout << "PCM3718->WriteBit: bit number out of range [1,8]!" << std::endl;
		return;
	}

	char byteToWrite = 0x00;
	if (address == PCM3718_DIG_OUT_LOW) byteToWrite = digOutLowByte;
	else if (address == PCM3718_DIG_OUT_HIGH) byteToWrite = digOutHighByte;

	// If bit is a 1, set the bit at bitNumber position
	if (bit) {
		//We have to OR the bit with the bit we want to write
		byteToWrite |= 0x01 << (bitNumber - 1);
	} else {
		//If it's a 0 we have to AND the bit with the bit we want to write
		//By ANDing with a pattern of 1's and a 0 where the bit we want to clear is
		byteToWrite &= ~(0x01 << (bitNumber - 1));
	}
	outb(byteToWrite, baseAddress + address);

	if (address == PCM3718_DIG_OUT_LOW) digOutLowByte = byteToWrite;
	else if (address == PCM3718_DIG_OUT_HIGH) digOutHighByte = byteToWrite;
}

void PCM3718::WriteByte(int address, unsigned char byte) {
	if (address < 0 || address >= PCM3718_BYTE_SIZE){
		std::cout << "PCM3718->WriteByte: address is out of range!" << std::endl;
		return;
	}

	if (address == PCM3718_DIG_OUT_LOW) digOutLowByte = byte;
	else if (address = PCM3718_DIG_OUT_HIGH) digOutHighByte = byte;
	outb(byte, baseAddress + address);
}

bool PCM3718::ReadBit(int address, int bitNumber)
{
	if (address < 0 || address >= PCM3718_BYTE_SIZE){
		std::cout << "PCM3718->ReadBit: address is out of range!" << std::endl;
		return 0;
	}

	if (bitNumber < 1 || bitNumber > 8){
		std::cout << "PCM3718->ReadBit: bit number out of range [1,8]!" << std::endl;
		return 0;
	}

	char readByte = (char)inb(baseAddress + address);
	readByte &= (0x01 << (bitNumber - 1));
	readByte >>= (bitNumber - 1);
	bool readBit = (bool)readByte;
	return readBit;
}

char PCM3718::ReadByte(int address)
{
	if (address < 0 || address >= PCM3718_BYTE_SIZE){
		std::cout << "PCM3718->ReadByte: address is out of range!" << std::endl;
		return 0;
	}
	return (char)inb(baseAddress + address);
}

int PCM3718::GetBaseAddress(){
	return baseAddress;
}

/*
	Reads from an analog channel within a specified range.
	Returns the voltage read from the selected channel as a double.
*/
double PCM3718::AnalogIn(unsigned char range, unsigned char channel){
	if (channel < 0 || channel >= 16) {
		std::cout << "PCM3718->AnalogIn: channel is out of range [0,15]!" << std::endl;
		return 0;
	}

	SetADRange(range);
	outb(0x00, baseAddress + PCM3718_AD_REGISTER);
	SetInputChannel(channel);
	outb(0x00, baseAddress);
	while (inb(baseAddress + PCM3718_AD_REGISTER) & 0x80);	//Keep looping until conversion is complete
	unsigned char adNibble = inb(baseAddress);
	unsigned char adByte = inb(baseAddress + PCM3718_AD_HIGH_BYTE);
	int adValue = ((adNibble >> 4) | (adByte << 4));
	return ConvertToVoltage(adValue);
}

/*
	Sets the range for the ADC to use for conversion.
	All possible ranges are defined in the header file under PCM3718_ADC_..
*/
void PCM3718::SetADRange(unsigned char range){
	unsigned char mask = 0x0F;
	rangeSelect = (range & mask); // Most significant nibble is N/A so doesn't matter if we overwrite it
	outb(rangeSelect, baseAddress + PCM3718_RANGE_REGISTER);
	usleep(5);
}

/*
	Sets the channel for the ADC to read from.
	Channel must be between 0 and 15 (will be overwritten if it is more than this).
*/
void PCM3718::SetInputChannel(unsigned char channel){
	// Set lower and higher nibbles to selected channel
	outb((channel | channel << 4), baseAddress + PCM3718_MUX_REGISTER);
	usleep(5);
}

// double PCM3718::ConvertToVoltage(int adVal) {
// 	double voltage = 0.0, maxVoltage = 0.0;
// 	unsigned char temp;
// 	bool unipolar = false;
// 	temp = (rangeSelect >> 2) & 0x01;
// 
// 	if (temp == 0) {	// If 3rd bit is 0, range is bipolar
// 		temp = (rangeSelect & 0x08) >> 3;
// 		if (temp == 0) {
// 			maxVoltage = 5.0;
// 			temp = (rangeSelect & 0x03);
// 			if (temp != 0) maxVoltage /= (2 * temp);
// 		} else maxVoltage = 10.0;
// 	} else {
// 		unipolar = true;
// 		maxVoltage = 10.0;
// 		temp = (rangeSelect & 0x03);
// 		if (temp != 0) maxVoltage /= (2 * temp);
// 	}
// 
// 	voltage = ((double)adVal / 4095.0);	//Normalise the value [0,1]
// 	if (!unipolar) {
// 		voltage *= 2;	// Set range to [0,2]
// 		voltage--;		// Set range to [-1,1]
// 	}
// 
// 	voltage *= maxVoltage;	// Set range to [-maxVoltage,maxVoltage] for bipolar
// 	return voltage;
// }

/*
double PCM3718::ConvertToVoltage(int adVal) {
	double voltage, maxVoltage;
	bool unipolar = false;
	switch (rangeSelect) {
	case PCM3718_ADC_BP_10V:
		maxVoltage = 10.0;
		break;
	case PCM3718_ADC_BP_5V:
		maxVoltage = 5.0;
		break;
	case PCM3718_ADC_BP_2_DOT_5V:
		maxVoltage = 2.5;
		break;
	case PCM3718_ADC_BP_1_DOT_25V:
		maxVoltage = 1.25;
		break;
	case PCM3718_ADC_BP_0_DOT_625V:
		maxVoltage = 0.625;
		break;
	case PCM3718_ADC_UP_10V:
		maxVoltage = 10.0;
		unipolar = true;
		break;
	case PCM3718_ADC_UP_5V:
		maxVoltage = 5.0;
		unipolar = true;
		break;
	case PCM3718_ADC_UP_2_DOT_5V:
		maxVoltage = 2.5;
		unipolar = true;
		break;
	case PCM3718_ADC_UP_1_DOT_25V:
		maxVoltage = 1.25;
		unipolar = true;
		break;
	}
	voltage = ((double)adVal / 4095.0);	//Normalise adVal to range [0,1]
	if (!unipolar) {	// If bipolar
		voltage *= 2.0;	// Set range to [0,2]
		voltage--;		// Set range to [-1,1]
	}
	voltage *= maxVoltage;	//Set range to [0, maxVoltage] for unipolar or [-maxVoltage,maxVoltage] for bipolar
	return voltage;
}*/

double PCM3718::ConvertToVoltage(int adVal){
	double voltage = 0.0;
	double max_voltage = 0.0;
	double min_voltage = 0.0;
	unsigned char temp = rangeSelect;
	bool unipolar = false;
	//Check bit 3 for unipolar/bipolar nature
	temp >>= 2;
	temp &= 0x01;
	if (temp == 0){ //Bipolar
		temp = rangeSelect;
		temp &= 0x08; //Extract highest bit of lower nibble
		temp >>= 3;
		if (temp == 0){
			min_voltage = -5.0;
			max_voltage = 5.0;
			temp = rangeSelect;
			temp &= 0x03;
			if (temp != 0){
				max_voltage /= (2*temp);
				min_voltage /= (2*temp);
			}
		} else { //+- 10
			max_voltage = 10.0;
			min_voltage = -10.0;
		}
	} else { //Unipolar
		unipolar = true;
		max_voltage = 10.0;
		temp = rangeSelect;
		temp &= 0x03; //Extract lower 2 bits
		if (temp != 0){
			max_voltage /= (2*temp);
		}
	}
	//now max and min voltage for range has been extracted, can do some maths to get adc value as a voltage
	voltage = adVal;
	voltage /= 4095.0; //Max value for ad_val
	if (!unipolar) {
		voltage *= 2;
		voltage--; //Set range to [-1,1]
	}
	
	voltage *= max_voltage; //Set range to [-max_voltage, max_voltage] for bipolar or [0,max_voltage] for unipolar
	return voltage;
}

std::ostream& operator<< (std::ostream& stream, PCM3718& p) {
	for (int i = 0; i < PCM3718_NUM_ANALOG_CHANNELS; i++) {
		stream << "Channel " << i << "\t";
	}
	stream << std::endl;
	for (int i = 0; i < PCM3718_NUM_ANALOG_CHANNELS; i++) {
		stream << p.AnalogIn(p.rangeSelect, i) << "\t";
	}
	return stream;
}
