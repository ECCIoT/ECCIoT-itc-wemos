// 
// 
// 

#include "TsButton.h"

TsButton::TsButton(uint8_t pinBtn, uint8_t normal_state = HIGH){
	pin = pinBtn;
	normalState = normal_state;

	//将引脚电位置于默认状态
	digitalWrite(pin, normalState);
	//设置引脚模式为输入模式
	pinMode(pin, INPUT);
	//初始化电位状态
	state = lastState = normalState;

	onKeyDown = 0;
	onKeyUp = 0;
	onKeyPress = 0;
}

uint8_t TsButton::readState(){
	return state;
}

void TsButton::updateState(){
	lastState = state;
	state = digitalRead(pin);

	//当前状态为正常电平，上一状态为正常电平，则不触发任何操作
	if (state == normalState && lastState == normalState){
		return;
	}
	//当前状态为正常电平，上一状态为非正常电平，则为按键弹起状态
	else if (state == normalState && lastState == !normalState){
		if (onKeyUp!=0)onKeyUp();
	}
	//当前状态为非正常电平，上一状态为正常电平，则为按键按下状态
	else if (state == !normalState && lastState == normalState){
		if (onKeyDown != 0)onKeyDown(); listener();
	}
	//当前状态为非正常电平，上一状态为非正常电平，则为按键持续按压状态
	else if (state == !normalState && lastState == !normalState){
		if (onKeyPress != 0)onKeyPress();
	}
}

void TsButton::addEvent(TsEventListener eventFunc){
	listener = eventFunc;
}

uint8_t TsButton::getPin(){
	return pin;
}
