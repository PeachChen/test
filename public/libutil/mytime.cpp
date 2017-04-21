#include "mytime.h"
#include <stdlib.h>

time_t CTime::m_tmt=NULL;
void CTime::GetCurrentTime(TimeFormat tf,char *addr,int len)
{
	time(&m_tmt);
	switch(tf)
	{
	case TF1:
		strftime(addr,len,"%Y-%m-%d",localtime(&m_tmt));
		break;
	case TF2:
		strftime(addr,len,"%H:%M:%S",localtime(&m_tmt));
		break;
	default:
		return;
	}	
}

int CTime::GetNumTime(TimeFormat tf)
{
	time(&m_tmt);
	char temp[8]={0};
	switch(tf)
	{
	case TF3:
		strftime(temp,8,"%H",localtime(&m_tmt));
		break;
	case TF4:
		strftime(temp,8,"%d",localtime(&m_tmt));
		break;
	default:
		return -1;
	}
	return atoi(temp);
}