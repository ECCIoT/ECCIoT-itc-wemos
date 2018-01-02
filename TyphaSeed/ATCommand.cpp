// 
// 
// 

#include "ATCommand.h"

//========================ATCommand===========================

CommandParameter::CommandParameter(String& strParam){
	param = strParam;
}

uint8_t CommandParameter::count(){
	//����������С�ڻ����1������'='�������������Ϊ0
	if (param.length() <= 1)
		return 0;
	else{
		//
		int index = 0;
		uint8_t c = 1;
		while (true){
			index = param.indexOf(',', index + 1);
			if (index != -1)
				c++;
			else
				return c;
		}
	}
}

String CommandParameter::get(uint8_t index){
	uint8_t pos = 0;
	while (index > 0){
		pos = param.indexOf(',', pos + 1);
		if (pos == -1) return "";
		index--;
	}

	
	uint8_t nextPos = param.indexOf(',', pos + 1);
	if (nextPos == -1){
		return param.substring(pos + 1);
	}
	else{
		return param.substring(pos + 1,nextPos);
	}
}


//============================================================


//=======================CommandItem==========================

CommandItem::CommandItem(String strName, CommandFunc func){
	name = strName;
	commandFunc = func;
}

CommandItem::CommandItem(const char * cpName, CommandFunc func){
	name = cpName;
	commandFunc = func;
}

String CommandItem::execute(CommandParameter param){
	return commandFunc(param);
}

String& CommandItem::getName(){
	return name;
}

//============================================================



//========================ATCommand===========================

void ATCommand::addCommandItem(CommandItem& item){
	commands.push_back(item);
}

void ATCommand::addCommandItems(CommandItem items[], int count){
	for (int i = 0; i < count; i++){
		addCommandItem(items[i]);
	}
}

String ATCommand::parse(String strCmd){
	//����ַ�����ʼ����AT+������ַ�������ATָ��
	if (!strCmd.startsWith("AT+"))
		return "ERROR";

	strCmd = strCmd.substring(3, strCmd.length());

	//����ATָ�������ÿһ��
	int count = commands.size();
	for (int i = 0; i < count; i++){
		//�ҵ���Ӧ��ָ������
		if (strCmd.startsWith(commands.at(i).getName())){
			//��ȡ��ָ��Ĳ���
			String param = strCmd.substring(commands[i].getName().length(), strCmd.length());
			//ִ�ж�Ӧ�Ļص�����
			CommandParameter cp(param);

			//ƴ�ӷ���ֵ
			String str;
			str = "+";
			str += commands.at(i).getName();
			str += ":";
			str += commands[i].execute(cp);
			str += "\r";
			return str;
			//return "+" + commands.at(i).getName() + ":" + commands[i].execute(cp);
		}
	}
	return "ERROR";
}

//============================================================