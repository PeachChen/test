#pragma once

#include <string>
using namespace std;

#define sLog CLog::GetInstance().WriteMainLog

class CLog
{	
public:
	static void Write(const char* filePath,bool append,const char * content,...);

	CLog();
	static CLog& GetInstance();
	void WriteMainLog(const char * content,...);
	string GetMainlogPath();
	void init(const char* path,const char* name,int hour);

private:
	int m_startHour;
	int m_endHour;
	int m_hour;
	int m_day;
	char m_path[64];
	char m_name[64];
	string m_mainLog;
};