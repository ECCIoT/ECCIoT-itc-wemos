/*
 Name:		TyphaSeed.ino
 Created:	11/26/2017 11:36:44 AM
 Author:	Landriesnidis
*/

#include "TsWaterSensor.h"
#include "TsLed.h"
#include "TsButton.h"
#include "pins_wemos_d1.h"

TsLed led(PIN_LED);
TsButton btn(PIN_D10,HIGH);

void btn_OnKeyDown(){
	led.reverse();
}

void setup() {
	Serial.begin(115200);
	Serial.println("Start");
	btn.onKeyDown = btn_OnKeyDown;
}

void loop() {
	btn.updateState();
	delay(50);
}
