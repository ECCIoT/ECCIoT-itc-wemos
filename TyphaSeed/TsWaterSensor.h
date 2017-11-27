// TsWaterSensor.h

#ifndef _TSWATERSENSOR_h
#define _TSWATERSENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#endif

class TsWaterSensor{
public:
	TsWaterSensor(uint8_t pinSensor, uint8_t normalState);
	uint8_t readState();
	void updateState();
	uint8_t getPin();
	//event
	void(*onWaterDetected)();
	void(*onWaterDisappeared)();
	void(*onStateChanged)(uint8_t val);
private:
	uint8_t pin, lastState, state, normalState;
};
