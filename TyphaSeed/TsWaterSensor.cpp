// 
// 
// 

#include "TsComponent.h"
#include "Arduino.h"

TsWaterSensor::TsWaterSensor(uint8_t pinBtn, uint8_t normal_state = HIGH){
	pin = pinBtn;
	normalState = normal_state;

	//�����ŵ�λ����Ĭ��״̬
	digitalWrite(pin, normalState);
	//��������ģʽΪ����ģʽ
	pinMode(pin, INPUT);
	//��ʼ����λ״̬
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

	//��ǰ״̬Ϊ������ƽ����һ״̬Ϊ������ƽ���򲻴����κβ���
	if (state == normalState && lastState == normalState){
		return;
	}
	//��ǰ״̬Ϊ������ƽ����һ״̬Ϊ��������ƽ����Ϊˮ��ʧ
	else if (state == normalState && lastState == !normalState){
		if (onWaterDisappeared != 0)onWaterDisappeared();
	}
	//��ǰ״̬Ϊ��������ƽ����һ״̬Ϊ������ƽ����Ϊ��⵽ˮ
	else if (state == !normalState && lastState == normalState){
		if (onWaterDetected != 0)onWaterDetected();
	}
	//��ǰ״̬����һ״̬��һ�£��򷵻ص�ǰ��״̬
	else if (state != lastState){
		if (onStateChanged != 0)onStateChanged(state);
	}

}

uint8_t TsWaterSensor::getPin(){
	return pin;
}
