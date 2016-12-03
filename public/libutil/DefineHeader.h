#pragma once

typedef unsigned int uint32;

//单件模式
#define SINGLETON_NODE(ClassName) \
	static ClassName& GetInstance() \
{\
	static ClassName s_##ClassName;\
	return s_##ClassName;\
}
