#include "TimeWheel.h"
#include <string.h>
#include <iostream>
#if !defined(__GNUC__)
#include <time.h>
#else
#include <sys/time.h>
#endif

const int TIME_LEVEL_SHIFT=6;
const int TIME_NEAR=256;
const int TIME_LEVEL=64;
const int TIME_NEAR_MASK=255;
const int TIME_NEAR_SHIFT=8;
const int TIME_LEVEL_MASK=63;

#if !defined(__GNUC__)
#define TIME_MAX_TIME UINT_MAX
#else
#define TIME_MAX_TIME 0xffffff*100+99
#endif

static uint32
get_cputime(){
	uint32 t;
#if !defined(__GNUC__)
	t=clock()/10;
#else
	struct timeval tv;
	gettimeofday(&tv,NULL);
	t=(uint32)(tv.tv_sec & 0xffffff)*100;
	t+=tv.tv_usec/10000;
#endif
	return t;
}


CTimeWheel::CTimeWheel()
{
	int i,j;
	memset(&tm,0,sizeof(tm));
	for(i=0;i<TIME_NEAR;i++)
	{
		link_clear(&(tm.near[i]));
	}
	
	for(i=0;i<4;i++)
		for(j=0;j<TIME_LEVEL-1;j++)
		{
			link_clear(&(tm.t[i][j]));
		}
}

void CTimeWheel::Init()
{
	tm.current=get_cputime();
	tm.starttime=get_cputime();
}

CTimeWheel::~CTimeWheel()
{
	int i,j;
	timer_node *next,*cur;
	for(i=0;i<TIME_NEAR;i++)
	{
		next=link_clear(&(tm.near[i]));
		while(next)
		{
			cur=next;
			next=next->next;
			delete cur;
		}
	}

	for(i=0;i<4;i++)
	{
		for(j=0;j<TIME_LEVEL-1;j++)
		{ 
		   	next=link_clear(&(tm.t[i][j]));
			while(next)
			{
				cur=next;
				next=next->next;
				delete cur;
			}
		}		
	}
}


void CTimeWheel::create_add_node(int expire,string str)
{
	timer_node* node=new timer_node();
	node->outstr=str;
	node->expire=expire+tm.time;
	add_node(node);
}

void CTimeWheel::add_node(timer_node *node)
{
	int time=node->expire;
	int current_time=tm.time;

	if((time|TIME_NEAR_MASK)==(current_time | TIME_NEAR_MASK))
	{
		link(&(tm.near[time&TIME_NEAR_MASK]),node);
		cout<<"add near str:"<<node->outstr.c_str()<<"  at:"<<(time&TIME_NEAR_MASK)<<"   curtime:"<<current_time<<endl;
	}
	else
	{
		int i;
		int mask=TIME_NEAR<<TIME_LEVEL_SHIFT;
		for(i=0;i<3;i++)
		{
			if((time |(mask-1))==(current_time | (mask-1)))
		   	{
				break; 
			}
		   	mask<<=TIME_LEVEL_SHIFT; 
		}
		link(&(tm.t[i][((time>>(TIME_NEAR_SHIFT+i*TIME_LEVEL_SHIFT))&TIME_LEVEL_MASK)-1]),node); 
		cout<<"add far str:"<<node->outstr.c_str()<<"  at i:"<<i<<" index:"<<(((time>>(TIME_NEAR_SHIFT+i*TIME_LEVEL_SHIFT))&TIME_LEVEL_MASK)-1)<<"   curtime:"<<current_time<<endl;
	}
}

void CTimeWheel::update()
{
	uint32 ct=get_cputime();
	if(ct!=tm.current)
	{
		int diff=ct>=tm.current?ct-tm.current:(TIME_MAX_TIME-tm.current+ct+1);
		tm.current=ct;
		for(int i=0;i<diff;i++)
		{
			timer_execute();
		}
	}
}

void CTimeWheel::timer_execute()
{
	int idx=tm.time & TIME_NEAR_MASK;
	timer_node *current,*temp;
	int mask,i,time;
	while(tm.near[idx].head.next)
	{
		current=link_clear(&tm.near[idx]);
		do
		{
			timeout(current->outstr);
			temp=current;
			current=current->next;
			delete temp;
		}while(current);
	}
	
	//注意这个++time的位置
	++tm.time;

	mask=TIME_NEAR;

	time=tm.time>>TIME_NEAR_SHIFT;
	i=0;
	while((tm.time & (mask-1))==0)
	{
		idx=time&TIME_LEVEL_MASK;
		if(idx!=0)
		{
			--idx;
			current=link_clear(&tm.t[i][idx]);
			while(current)
			{
				temp=current->next;
				add_node(current);
				current=temp;
			}
			break;
		}
		mask<<=TIME_LEVEL_SHIFT;
		time>>=TIME_LEVEL_SHIFT;
		++i;
		if(i>3)
		  break;
	}
}

void CTimeWheel::timeout(string str)
{
	//do something
	std::cout<<str<<"  "<<tm.time<<endl;
}
