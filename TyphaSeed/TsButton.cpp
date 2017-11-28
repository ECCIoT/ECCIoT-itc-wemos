// 
// 
// 

#include "TsButton.h"

TsButton::TsButton(uint8_t pinBtn, uint8_t normal_state = HIGH){
	pin = pinBtn;
	normalState = normal_state;

	//�����ŵ�λ����Ĭ��״̬
	digitalWrite(pin, normalState);
	//��������ģʽΪ����ģʽ
	pinMode(pin, INPUT);
	//��ʼ����λ״̬
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

	//��ǰ״̬Ϊ������ƽ����һ״̬Ϊ������ƽ���򲻴����κβ���
	if (state == normalState && lastState == normalState){
		return;
	}
	//��ǰ״̬Ϊ������ƽ����һ״̬Ϊ��������ƽ����Ϊ��������״̬
	else if (state == normalState && lastState == !normalState){
		if (onKeyUp!=0)onKeyUp();
	}
	//��ǰ״̬Ϊ��������ƽ����һ״̬Ϊ������ƽ����Ϊ��������״̬
	else if (state == !normalState && lastState == normalState){
		if (onKeyDown != 0)onKeyDown(); listener();
	}
	//��ǰ״̬Ϊ��������ƽ����һ״̬Ϊ��������ƽ����Ϊ����������ѹ״̬
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
