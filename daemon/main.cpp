#include <sys/types.h>
#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

extern int errno;

//守护进程
void daemonize()
{
	int pid=fork();
	
	if(pid<0)                   //出错
	  exit(-1);
	
	if(pid!=0)                    //父进程直接退出
		exit(0);
	else                          //子进程
	{
		printf("child pid is %d\n",(int)getpid());
		int sid=setsid();
		if(sid==-1)               //创建会话失败 退出
		{
			printf("setsid failed\n error:%s",strerror(errno));
			exit(-1);
		}
		pid=fork();               //setsid调用成功后 子进程变成了 会话首进程 可能再次获得终端 所以我们再fork一次
		if(pid<0)
		  exit(-1);
		else if(pid!=0)           //第一子进程退出
		  exit(0);

		printf("child child pid is %d\n",(int)getpid());
		umask(0);                 //重设文件创建掩模

		if(chdir("/")==-1)        //修改工作目录
		{
			printf("chdir failed\n");
		   	exit(-1);
		}
		
		rlimit rl;
		if(getrlimit(RLIMIT_NOFILE,&rl)<0)
		{
			printf("can't get file limit!");
			exit(-1);
		}
		if(rl.rlim_max==RLIM_INFINITY)
			rl.rlim_max=1024;
		for(int i=0;i<rl.rlim_max;i++)//关闭继承来的文件描述符
		  close(i);
		
		open("/dev/null",O_RDWR); //标准输入输出到null
		dup(0);
		dup(0);
	}
}


int main()
{
	int pid=getpid();
	printf("father pid is %d\n",pid);
	daemonize();
	static int m=0;
	while(m<10)
	{
		sleep(10);
		m+=1;

	}
	return 0;
}
