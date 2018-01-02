// TsComponent.h

#ifndef _TSCONTROLLER_h
#define _TSCONTROLLER_h


#include <functional>
#include "Arduino.h"


//所有TyphaSeed中电元封装类的基类
class TsComponent{
public:
	typedef std::function<void(void)> TsEventListener;
};


class TsButton :TsComponent{
public:
	TsButton(uint8_t pinBtn, uint8_t normal_state);
	uint8_t readState();
	void updateState();
	uint8_t getPin();
	void addEvent(TsEventListener eventFunc);
	//event
	void(*onKeyDown)();
	void(*onKeyUp)();
	void(*onKeyPress)();
private:
	TsEventListener listener;
	uint8_t pin, lastState, state, normalState;
};


class TsLed :TsComponent{
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

#endif



