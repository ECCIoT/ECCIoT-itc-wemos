// 
// 
// 

#include "JsonConfig.h"
#include "ArduinoJson.h"
#include "FS.h"

JsonConfig::JsonConfig(String filename, JsonConfigCallBackFunc errorFunc){
	fileName = filename;
	errorCallback = errorFunc;
}

bool JsonConfig::saveConfig(JsonObject& json){
	//���ļ���д��ģʽ��
	File configFile = SPIFFS.open(fileName, "w");
	//�쳣���:д��ģʽ�޷����ļ����������4
	if (!configFile) {
		String msg = "Failed to open config file for writing";
		errorCallback(4,msg);
		return false;
	}

	json.printTo(configFile);
	return true;
}


JsonObject JsonConfig::getJsonObject(){

	//���ļ�����ȡģʽ��
	File configFile = SPIFFS.open(fileName, "r");
	//�쳣���:�޷����ļ����������1
	if (!configFile) {
		String msg = "Failed to open config file";
		errorCallback(1, msg);
		return;
	}

	//��ȡ�ļ���С
	size_t size = configFile.size();
	//�쳣���:�ļ����󣬴������2
	if (size > 1024) {
		String msg = "Config file size is too large";
		errorCallback(2, msg);
		return;
	}

	std::unique_ptr<char[]> buf(new char[size]);

	configFile.readBytes(buf.get(), size);

	StaticJsonBuffer<200> jsonBuffer;
	return jsonBuffer.parseObject(buf.get());

}

bool JsonConfig::isParseSuccess(JsonObject& json){
	//�쳣���:����Json�ַ���ʧ��
	if (!json.success()) {
		String msg = "Failed to parse config file";
		errorCallback(3, msg);
		return false;
	}
	return true;
}
