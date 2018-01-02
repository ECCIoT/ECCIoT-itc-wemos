// 
// 
// 

#include "TsComponent.h"
#include "Arduino.h"

TsWaterSensor::TsWaterSensor(uint8_t pinBtn, uint8_t normal_state = HIGH){
	pin = pinBtn;
	normalState = normal_state;

	//将引脚电位置于默认状态
	digitalWrite(pin, normalState);
	//设置引脚模式为输入模式
	pinMode(pin, INPUT);
	//初始化电位状态
	state = lastState = normalState;

	onWaterDetected = 0;
	onWaterDisappeared = 0;
	onStateChanged = 0;
}

uint8_t TsWaterSensor::readState(){
	return state;
}

void TsWaterSensor::updateState(){
	lastState = state;
	state = digitalRead(pin);

	//当前状态为正常电平，上一状态为正常电平，则不触发任何操作
	if (state == normalState && lastState == normalState){
		return;
	}
	//当前状态为正常电平，上一状态为非正常电平，则为水消失
	else if (state == normalState && lastState == !normalState){
		if (onWaterDisappeared != 0)onWaterDisappeared();
	}
	//当前状态为非正常电平，上一状态为正常电平，则为检测到水
	else if (state == !normalState && lastState == normalState){
		if (onWaterDetected != 0)onWaterDetected();
	}
	//当前状态与上一状态不一致，则返回当前新状态
	else if (state != lastState){
		if (onStateChanged != 0)onStateChanged(state);
	}

}

uint8_t TsWaterSensor::getPin(){
	return pin;
}
