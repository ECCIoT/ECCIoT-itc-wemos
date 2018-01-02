// 
// 
// 

#include "TsComponent.h"
#include "Arduino.h"

TsLed::TsLed(uint8_t pinLed){
	pin = pinLed;
	pinMode(pin, OUTPUT);
}

void TsLed::setState(uint8_t val){
	digitalWrite(pin, val);
}

void TsLed::setState(bool b){
	digitalWrite(pin, b?HIGH:LOW);
}

void TsLed::setValue(uint16_t val){
	analogWrite(pin, val);
}

void TsLed::reverse(){
	digitalWrite(pin, !digitalRead(pin));
}

uint8_t TsLed::getPin(){
	return pin;
}
