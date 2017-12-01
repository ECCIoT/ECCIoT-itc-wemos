// JsonConfig.h

#ifndef _JSONCONFIG_h
#define _JSONCONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ArduinoJson.h"
#include <functional>

#endif



class JsonConfig{
public:
	//�ص�������FuncName(uint8_t error_code,String& error_msg)
	typedef std::function<bool(uint8_t,String&)> JsonConfigCallBackFunc;

	JsonConfig(const String &filename, std::function<bool(uint8_t, String&)> errorFunc);
	
	//����Json���ݵ������ļ�
	bool saveConfig(JsonObject& json);

	//��ȡ�����ļ��е�Json����
	JsonObject& getConfigJson(bool bAutoCreate = true);

	//�ж������Ƿ�ɹ�����ΪJson����
	bool isParseSuccess(JsonObject& json);

	//ɾ�������ļ�
	bool deleteConfig();

	//�ж������ļ��Ƿ����
	bool isExist();
private:
	String fileName;
	JsonConfigCallBackFunc errorCallback;
};