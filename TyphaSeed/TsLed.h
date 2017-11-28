// TsLed.h

#ifndef _TSLED_h
#define _TSLED_h

#include "TsComponent.h"
//
class TsLed:TsComponent{
public:
	TsLed(uint8_t pinLed);
	void setState(uint8_t val);
	void setState(bool b);
	void setValue(uint16_t val);
	//引脚电平反转
	void reverse();
	uint8_t getPin();
private:
	uint8_t pin;
};

#endif


