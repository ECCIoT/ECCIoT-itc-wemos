// JsonConfig.h

#ifndef _JSONCONFIG_h
#define _JSONCONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ArduinoJson.h"

#endif

class JsonConfig{
public:
	//�ص�������FuncName(uint8_t error_code,String& error_msg)
	typedef std::function<void(uint8_t,String&)> JsonConfigCallBackFunc;

	JsonConfig(const String &filename, JsonConfigCallBackFunc errorFunc);
	
	//����Json���ݵ������ļ�
	bool saveConfig(JsonObject& json);

	//��ȡ�����ļ��е�Json����
	JsonObject getJsonObject();

	//�ж������Ƿ�ɹ�����ΪJson����
	bool isParseSuccess(JsonObject& json);
private:
	String fileName;
	JsonConfigCallBackFunc errorCallback;
};