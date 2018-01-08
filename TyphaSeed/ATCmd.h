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
	//���캯��
	CommandParameter(String& strParam);
	//��ȡ��������
	uint8_t count();
	//��ȡ��index��������
	String get(uint8_t index);
private:
	String param;
};

class CommandItem{
public:
	//�ص���������
	typedef std::function<String(CommandParameter)> CommandFunc;
	//���캯��
	CommandItem(String strName, CommandFunc func);
	CommandItem(const char * cpName, CommandFunc func);
	//ִ�лص�����
	String execute(CommandParameter param);
	//��ȡ��������
	String& getName();
private:
	String name;
	CommandFunc commandFunc;
};

class ATCommand{
public:
	//���ָ����
	void addCommandItem(CommandItem& item);
	//���ָ��������
	void addCommandItems(CommandItem items[], int count);
	//����ATָ��
	String parse(String strCmd);
private:
	vector<CommandItem> commands;
};
#endif

