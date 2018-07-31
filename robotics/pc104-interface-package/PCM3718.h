#ifndef PCM3718_H
#define PCM3718_H
#include <iostream>

#define PCM3718_BYTE_SIZE 16
#define PCM3718_NUM_ANALOG_CHANNELS 2
#define PCM3718_RANGE_REGISTER 0x01
#define PCM3718_AD_HIGH_BYTE 0x01
#define PCM3718_MUX_REGISTER 0x02
#define PCM3718_DIG_OUT_LOW 0x03
#define PCM3718_AD_REGISTER 0x08
#define PCM3718_DIG_OUT_HIGH 0x0B

// ADC Range Values
// Naming system:
// <Device_Name>_<Type_Of_Constant>_<UniPolar/BiPolar>_<Voltage_Range>
// Example:
// PCM3718_ADC_BP_1_DOT_25V is the constant for setting the ADC range on
// the PCM3718 to 1.25V bipolar, so the range is [-1.25,1.25]
#define PCM3718_ADC_BP_10V 0x08
#define PCM3718_ADC_BP_5V 0x0
#define PCM3718_ADC_BP_2_DOT_5V 0x01
#define PCM3718_ADC_BP_1_DOT_25V 0x02
#define PCM3718_ADC_BP_0_DOT_625V 0x03
#define PCM3718_ADC_UP_10V 0x04
#define PCM3718_ADC_UP_5V 0x05
#define PCM3718_ADC_UP_2_DOT_5V 0x06
#define PCM3718_ADC_UP_1_DOT_25V 0x07

class PCM3718 {
private:
	unsigned char digOutLowByte;
	unsigned char digOutHighByte;
	int baseAddress;
	unsigned char rangeSelect;
	
	void SetInputChannel(unsigned char channel);
	double ConvertToVoltage(int adVal);
public:
	PCM3718(int baseAddress);
	void WriteBit(int address, int bitNumber, bool bit);
	void WriteByte(int address, unsigned char byte);
	bool ReadBit(int address, int bitNumber);
	char ReadByte(int address);
	int GetBaseAddress();
	double AnalogIn(unsigned char range, unsigned char channel);
	void SetADRange(unsigned char range);
	friend std::ostream& operator<< (std::ostream& stream, PCM3718& p);
};
#endif
