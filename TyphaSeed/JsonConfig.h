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
	//回调函数，FuncName(uint8_t error_code,String& error_msg)
	typedef std::function<bool(uint8_t,String&)> JsonConfigCallBackFunc;

	JsonConfig(const String &filename, std::function<bool(uint8_t, String&)> errorFunc);
	
	//保存Json数据到配置文件
	bool saveConfig(JsonObject& json);

	//获取配置文件中的Json对象
	JsonObject& getConfigJson(bool bAutoCreate = true);

	//判断数据是否成功解析为Json数据
	bool isParseSuccess(JsonObject& json);

	//删除配置文件
	bool deleteConfig();

	//判断配置文件是否存在
	bool isExist();
private:
	String fileName;
	JsonConfigCallBackFunc errorCallback;
};