#pragma once

#include <time.h>

enum TimeFormat
{
	TF1,// 2001-09-01
	TF2,// 10:25:24
	TF3,// hour(0-23)
	TF4,// days(1-31)
};
class CTime
{
public:
	static void GetCurrentTime(TimeFormat tf,char *addr,int len);
	static int GetNumTime(TimeFormat tf);
private:
	static time_t m_tmt;
};