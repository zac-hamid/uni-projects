#include <iostream>
#include <stdlib.h>
#include <sys/io.h>
#include "PCM3718.h"
#include "MSIP404.h"
#include "DAC06.h"

using namespace std;

int main() {
// 	if (ioperm(0x300,16,1) != 0){
// 		throw "Unable to get I/O permissions for PCM3718!";
// 		return 1;
// 	}
	PCM3718 p(0x300);
	MSIP404 m(0x200, 0x800);
	DAC06 d(0x320, DAC06_RANGE_5V_BP);

	p.WriteByte(0x03, 0xA0);
	p.WriteByte(0x0B, 0xF0);
	sleep(3);
	cout << "CH0: " << p.AnalogIn(PCM3718_ADC_BP_5V, 0) << endl;
	cout << "CH1: " << p.AnalogIn(PCM3718_ADC_BP_5V, 1) << endl;
	//cout << p << endl;
	p.WriteByte(0x03, 0x33);
	p.WriteByte(0x0B, 0x0F);
	sleep(3);
	cout << p.AnalogIn(PCM3718_ADC_BP_5V, 0) << endl;
	//cout << p << endl;
	//d.AnalogOut(0, -1.28);
	int x = 0;
	long encoder_val;
	//while(1){
	//	if (m.DetectIndexPulse()) cout << "INDEX PULSE!" << endl;
	//	encoder_val = m.EncoderRead(0);
	//	//cout << encoder_val << endl;
	//}
	return EXIT_SUCCESS;
}
