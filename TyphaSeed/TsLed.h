// TsLed.h

#ifndef _TSLED_h
#define _TSLED_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


#endif

class TsLed{
public:
	TsLed(uint8_t pinLed);
	void setState(uint8_t val);
	void setState(bool b);
	void setValue(uint16_t val);
	void reverse();
	uint8_t getPin();
private:
	uint8_t pin;
};
