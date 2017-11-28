// TsComponent.h

#ifndef _TSCONTROLLER_h
#define _TSCONTROLLER_h

#include <functional>

//所有TyphaSeed中电元封装类的基类
class TsComponent{
public:
	typedef std::function<void(void)> TsEventListener;
};

#endif



