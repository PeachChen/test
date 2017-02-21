#include <iostream>
#include <pthread.h>
#include <deque>
#include <stdlib.h>

using namespace std;


deque<int> m_deque;
pthread_mutex_t work_mutex;
pthread_cond_t cond;

bool m_running=true;

void* push(void* msg)
{
	while(m_running)
	{
		pthread_mutex_lock(&work_mutex);
		int m=rand();
		m_deque.push_back(m);
		cout<<(char*)msg<<" push "<<m<<endl;
		pthread_mutex_unlock(&work_mutex);
		pthread_cond_signal(&cond);
		sleep(1);
	}
	return msg;
}

void* pop(void* msg)
{
	while(m_running)
	{
		pthread_mutex_lock(&work_mutex);
		while(m_deque.empty())
		{
			//解锁work_mutex 并阻塞本线程
			//如果条件成立 又加锁work_mutex 执行
			pthread_cond_wait(&cond, &work_mutex);
		}
		int m=m_deque.front();
		m_deque.pop_front();
		cout<<(char*)msg<<" pop "<<m<<endl;
		pthread_mutex_unlock(&work_mutex);
	}
	return msg;
}

int main()
{
	pthread_t tId1,tId2;
	char msg[]="thread 1";
	char msg1[]="thread 2";
	void *status;

	pthread_mutex_init(&work_mutex,NULL);
	pthread_cond_init(&cond, NULL);
	int ret=pthread_create(&tId1,NULL,push,(void*)msg);
	if(ret!=0)
	{
		cout<<msg<<"create thread faild"<<endl;
	}

	ret=pthread_create(&tId2,NULL,pop,(void*)msg1);
	if(ret!=0)
	{
		cout<<msg1<<"create thread faild"<<endl;
	}
	sleep(10);
	m_running=false;
	pthread_join(tId1,&status);
	cout<<(char*)status<<" end!!!!!\n";
	pthread_join(tId2,&status);
	cout<<(char*)status<<" end!!!!!\n";
    //线程2会结束不了 这儿不管	
	pthread_mutex_destroy(&work_mutex);
	pthread_cond_destroy(&cond);
	return 0;	
}
