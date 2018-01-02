// ATCommand.h

#ifndef _ATCOMMAND_h
#define _ATCOMMAND_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <vector>
#include <functional>
using namespace std;


class CommandParameter{
public:
	//构造函数
	CommandParameter(String& strParam);
	//获取参数个数
	uint8_t count();
	//获取第index个参数项
	String get(uint8_t index);
private:
	String param;
};

class CommandItem{
public:
	//回调函数类型
	typedef std::function<String(CommandParameter)> CommandFunc;
	//构造函数
	CommandItem(String strName, CommandFunc func);
	CommandItem(const char * cpName, CommandFunc func);
	//执行回调函数
	String execute(CommandParameter param);
	//获取命令名称
	String& getName();
private:
	String name;
	CommandFunc commandFunc;
};

class ATCommand{
public:
	//添加指令项
	void addCommandItem(CommandItem& item);
	//添加指令项数组
	void addCommandItems(CommandItem items[], int count);
	//解析AT指令
	String parse(String strCmd);
private:
	vector<CommandItem> commands;
};
#endif

