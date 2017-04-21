#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "mytime.h"

CLog& CLog::GetInstance()
{
	static CLog s_log;
	return s_log;
}

CLog::CLog()
{	
	memset(m_path,0,64);
	memset(m_name,0,64);
}

void CLog::Write(const char* filePath,bool append,const char * content,...)
{
	if(filePath==NULL || content==NULL)
		return;
	FILE* p=::fopen(filePath,append?"a":"w");
	if(p==NULL) return;
	char str[1024]={0};
	CTime::GetCurrentTime(TF2,str,20);
	strcat( str, ":" );
	va_list va;
	va_start(va,content);
	vsprintf(str+strlen(str),content,va);
	va_end(va);
	strcat( str, "\n" );
	printf("%s",str);
	::fwrite(str,1,strlen(str),p);
	::fclose(p);
}

void CLog::WriteMainLog(const char * content,...)
{	
	string logPath=GetMainlogPath();
	FILE* p=::fopen(logPath.c_str(),"a");
	if(p==NULL) return;
	char str[1024]={0};
	CTime::GetCurrentTime(TF2,str,20);
	strcat( str, ":" );
	va_list va;
	va_start(va,content);
	vsprintf(str+strlen(str),content,va);
	va_end(va);
	strcat( str, "\n" );
	printf("%s",str);
	::fwrite(str,1,strlen(str),p);
	::fclose(p);
}

string CLog::GetMainlogPath()
{
	int tempDay=CTime::GetNumTime(TF4);
	int tempHour=CTime::GetNumTime(TF3);
	if(tempDay==m_day)
	{
		if(tempHour>=m_endHour)
		{
			m_startHour=m_endHour;
			m_endHour=m_endHour+m_hour;
			char str[20]={0};
			CTime::GetCurrentTime(TF1,str,20);
			char temppath[50]={0};
			sprintf(temppath,"%s%s-%d-%d-%s",m_path,str,m_startHour,m_endHour,m_name);
			m_mainLog=temppath;			
		}
	}
	else
	{
		m_startHour=0;
		m_endHour=m_hour;
		char str[20]={0};
		CTime::GetCurrentTime(TF1,str,20);
		char temppath[50]={0};
		sprintf(temppath,"%s%s-%d-%d-%s",m_path,str,m_startHour,m_endHour,m_name);
		m_mainLog=temppath;		
	}
	return m_mainLog;
}

void CLog::init(const char* path,const char* name,int hour)
{
	memcpy(m_path,path,strlen(path));
	memcpy(m_name,name,strlen(name));
	m_hour=hour;
	int curhour=CTime::GetNumTime(TF3);
	m_day=CTime::GetNumTime(TF4);
	m_startHour=(curhour/hour)*hour;
	m_endHour=m_startHour+hour;
	char str[20]={0};
	CTime::GetCurrentTime(TF1,str,20);
	char temppath[50]={0};
	sprintf(temppath,"%s%s-%d-%d-%s",path,str,m_startHour,m_endHour,name);
	m_mainLog=temppath;
}