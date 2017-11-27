#include "Arduino.h"

class Button{

public :
	Button(uint8_t pin){
		intPin = pin;
		pinMode(pin, INPUT);
	}

	uint8_t getPin(){
		return intPin;
	}

	int getState(){
		return digitalRead(intPin);
	}

	bool isHigh(){
		return getState() == HIGH;
	}
private :
	uint8_t intPin;
};