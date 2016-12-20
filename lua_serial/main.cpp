#include "LuaScript.h"
#include <iostream>

using namespace std;

int main()
{
	if(!CLuaScript::GetInstance().LoadFile("test.lua") )
	{
		cout<<"loadFile faild"<<endl;
	}
	return 0;
}
