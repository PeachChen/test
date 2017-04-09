#include <boost/shared_ptr.hpp>
#include <iostream>

using namespace std;

class A
{
	public:
		A()
		{cout<<"A construct"<<endl;}
		~A()
		{
			cout<<"A destroy"<<endl;
		}
};

int main()
{
	boost::shared_ptr<A> d;
	//d=boost::shared_ptr<A>(new A());
	//当不得不分开声明和定义时候reset是更优雅的写法 reset不带参数是引用计数减一
	//带参数是减一的同时 用参数新构造一个shared_ptr对象
	d.reset(new A());	
	if(d)
	{
		cout<<"d true"<<endl;
	}
	else
	{
		cout<<"d false"<<endl;
	}
	return 0;
}
