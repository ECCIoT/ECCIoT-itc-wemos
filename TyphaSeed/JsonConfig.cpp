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
	//打开文件（写入模式）
	File configFile = SPIFFS.open(fileName, "w");
	//异常检测:写入模式无法打开文件，错误代码4
	if (!configFile) {
		String msg = "Failed to open config file for writing";
		errorCallback(4,msg);
		return false;
	}

	json.printTo(configFile);
	return true;
}


JsonObject JsonConfig::getJsonObject(){

	//打开文件（读取模式）
	File configFile = SPIFFS.open(fileName, "r");
	//异常检测:无法打开文件，错误代码1
	if (!configFile) {
		String msg = "Failed to open config file";
		errorCallback(1, msg);
		return;
	}

	//读取文件大小
	size_t size = configFile.size();
	//异常检测:文件过大，错误代码2
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
	//异常检测:解析Json字符串失败
	if (!json.success()) {
		String msg = "Failed to parse config file";
		errorCallback(3, msg);
		return false;
	}
	return true;
}
