// TsButton.h

#ifndef _TSBUTTON_h
#define _TSBUTTON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


#endif

class TsButton{
public:
	TsButton(uint8_t pinBtn, uint8_t normal_state);
	uint8_t readState();
	void updateState();
	uint8_t getPin();
	//event
	void(*onKeyDown)();
	void(*onKeyUp)();
	void(*onKeyPress)();
private:
	uint8_t pin, lastState, state, normalState;
};
