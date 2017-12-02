// 
// 
// 

#include "JsonConfig.h"
#include "ArduinoJson.h"
#include "FS.h"

//重启
void(*reset0) (void) = 0;

//显示重启提示并延时重启
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
	//打开文件（写入模式）
	File configFile = SPIFFS.open(fileName, "w");
	//异常检测:写入模式无法打开文件，错误代码4
	if (!configFile) {
		String msg = "Failed to open config file for writing";
		if(!errorCallback(4,msg))
			return false;
	}
	json.printTo(configFile);
	return true;
}


JsonObject& JsonConfig::getConfigJson(bool bAutoCreate){

	//如果文件不存在且使用自动创建时
	if (bAutoCreate && isExist() == false){
		return jsonBuffer.createObject();
	}
	else{
		//打开文件（读取模式）
		File configFile = SPIFFS.open(fileName, "r");

		//异常检测:无法打开文件，错误代码1
		if (!configFile) {
			String msg = "Failed to open config file";
			if (!errorCallback(1, msg))
				resetPrompt();
		}

		//读取文件大小
		size_t size = configFile.size();
		//异常检测:文件过大，错误代码2
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
	//异常检测:解析Json字符串失败
	if (!json.success()) {
		String msg = "Failed to parse config file";
		errorCallback(3, msg);
		return false;
	}
	return true;
}