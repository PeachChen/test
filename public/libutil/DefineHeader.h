#pragma once

//类型定义
typedef unsigned int uint32;
typedef unsigned int uint;
typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned long long uint64;
typedef long long int64;

//标准库头文件
#include <string.h>
#include <string>
#include <vector>
using namespace std;

extern "C"
{
	#include <assert.h>
}
#define Assert(a)

//单件模式
#define SINGLETON_NODE(ClassName) \
	static ClassName& GetInstance() \
{\
	static ClassName s_##ClassName;\
	return s_##ClassName;\
}


#if !defined(SAFE_DELETE)
#define SAFE_DELETE(p) if((p)) {delete (p);(p)=0;} 
#endif

#if !defined(SAFE_DELETE_ARRAY)
#define SAFE_DELETE_ARRAY(p) if((p)) {delete [] (p);(p)=0;} 
#endif

