#include <iostream>
#include <pthread.h>

using namespace std;

//互斥锁 适用于对共享资源加锁
int ticket=50;
pthread_mutex_t work_mutex;
void* Thread_func(void *arg)
{

	while(ticket>0)
	{
		pthread_mutex_lock(&work_mutex);
		if(ticket>0)
		{
		cout<<(char*)arg<<"ticket:"<<ticket<<endl;
		ticket--;
		}
		pthread_mutex_unlock(&work_mutex);
		sleep(1);	
	}
	return arg;
}

int main()
{
	pthread_t tId1,tId2;
	char msg[]="thread 1";
	char msg1[]="thread 2";
	void *status;

	pthread_mutex_init(&work_mutex,NULL);
	int ret=pthread_create(&tId1,NULL,Thread_func,(void*)msg);
	if(ret!=0)
	{
		cout<<msg<<"create thread faild"<<endl;
	}

	ret=pthread_create(&tId2,NULL,Thread_func,(void*)msg1);
	if(ret!=0)
	{
		cout<<msg1<<"create thread faild"<<endl;
	}
	
	pthread_join(tId1,&status);
	cout<<(char*)status<<" end!!!!!\n";
	pthread_join(tId2,&status);
	cout<<(char*)status<<" end!!!!!\n";
	
	pthread_mutex_destroy(&work_mutex);
	return 0;	
}
