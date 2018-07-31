#ifndef DAC06_H
#define DAC06_H

#define DAC06_BYTE_SIZE 12
#define DAC06_NUM_CHANNELS 6

#define DAC06_CHANNEL_0 0
#define DAC06_CHANNEL_1 1
#define DAC06_CHANNEL_2 2
#define DAC06_CHANNEL_3 3
#define DAC06_CHANNEL_4 4
#define DAC06_CHANNEL_5 5

//DAC Ranges
#define DAC06_RANGE_10V_BP 0
#define DAC06_RANGE_5V_BP 1
#define DAC06_RANGE_10V_UP 2
#define DAC06_RANGE_5V_UP 3

class DAC06 {
private:
	int baseAddress;
	unsigned char range;
public:
	void AnalogOut(unsigned char channel, double value);
	int GetBaseAddress();
	void setDACRange(unsigned char range);
	DAC06(int baseAddress, unsigned char range);
};

#endif
