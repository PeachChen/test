#include "TimeWheel.h"
#include <iostream>
using namespace std;

int main()
{
	
	CTimeWheel::GetInstance().Init();
	CTimeWheel::GetInstance().create_add_node(200,"first add 200");
	CTimeWheel::GetInstance().create_add_node(500,"first add 500");
	bool add=false;
	while(true)
	{
		CTimeWheel::GetInstance().update();
		int i=11000000;
		while(i--)
		{
			if(i==5248663 and !add)
			{
			  	CTimeWheel::GetInstance().create_add_node(300,"secend add 300");
				add=true;		
			}
		}
	}
	cout<<"wo qu"<<endl;
	return 0;
}
