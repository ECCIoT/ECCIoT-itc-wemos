// 
// 
// 

#include "JsonConfig.h"
#include "ArduinoJson.h"
#include "FS.h"

//����
void(*reset0) (void) = 0;

//��ʾ������ʾ����ʱ����
void resetPrompt(int delayTime = 3000){
	Serial.println("<!> This board is rebooting...");
	delay(delayTime);
	reset0();
}


JsonConfig::JsonConfig(const String &filename, JsonConfigCallBackFunc errorFunc){
	fileName = filename;
	errorCallback = errorFunc;

	if (!SPIFFS.begin()) {
		String msg = "Failed to mount file system";
		if (!errorCallback(0, msg))
			resetPrompt();
	}
}

bool JsonConfig::saveConfig(JsonObject& json){
	//���ļ���д��ģʽ��
	File configFile = SPIFFS.open(fileName, "w");
	//�쳣���:д��ģʽ�޷����ļ����������4
	if (!configFile) {
		String msg = "Failed to open config file for writing";
		if(!errorCallback(4,msg))
			return false;
	}
	json.printTo(configFile);
	return true;
}


JsonObject& JsonConfig::getConfigJson(bool bAutoCreate){

	//����ļ���������ʹ���Զ�����ʱ
	if (bAutoCreate && isExist() == false){
		return jsonBuffer.createObject();
	}
	else{
		//���ļ�����ȡģʽ��
		File configFile = SPIFFS.open(fileName, "r");

		//�쳣���:�޷����ļ����������1
		if (!configFile) {
			String msg = "Failed to open config file";
			if (!errorCallback(1, msg))
				resetPrompt();
		}

		//��ȡ�ļ���С
		size_t size = configFile.size();
		//�쳣���:�ļ����󣬴������2
		if (size > 1024) {
			String msg = "Config file size is too large";
			if (!errorCallback(2, msg))
				resetPrompt();
		}

		std::unique_ptr<char[]> buf(new char[size]);

		configFile.readBytes(buf.get(), size);

		return jsonBuffer.parseObject(buf.get());
	}
	

	

}

bool JsonConfig::deleteConfig(){
	return SPIFFS.remove(fileName);
}

bool JsonConfig::isExist(){
	return SPIFFS.exists(fileName);
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