// TsComponent.h

#ifndef _TSCONTROLLER_h
#define _TSCONTROLLER_h

#include <functional>

//����TyphaSeed�е�Ԫ��װ��Ļ���
class TsComponent{
public:
	typedef std::function<void(void)> TsEventListener;
};

#endif



