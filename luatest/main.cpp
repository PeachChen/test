#include <iostream>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace std;

int L_ccprint(lua_State* L)
{
	cout<<"stack:"<<lua_gettop(L)<<endl;
	lua_pushnil(L);
	//lua_next()会处理自己push的值 保证栈原样
	while(lua_next(L,-2)!=0)
	{
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		//printf("%s - %s\n",lua_typename(L, lua_type(L, -2)),lua_typename(L, lua_type(L, -1)));
		/* removes 'value'; keeps 'key' for next iteration */
		//lua_pop(L, 1);
		if(lua_type(L,-2)!=LUA_TNUMBER)
		{
			lua_pop(L,1);
			continue;
		}
		lua_pushnil(L);
		while(lua_next(L,-2)!=0)
		{
			printf("%s = %s\n",lua_tostring(L,-2),lua_tostring(L,-1));
			lua_pop(L,1);
		}
		lua_pop(L,1);
	}
	cout<<"stack:"<<lua_gettop(L)<<endl;
	
	lua_pushstring(L,"teamskills");
	lua_rawget(L,-2);
	cout<<"stack:"<<lua_gettop(L)<<endl;
	if(lua_type(L,-1)==LUA_TTABLE)
	{
		lua_pushnil(L);
		while(lua_next(L,-2)!=0)
		{
			int a=lua_tonumber(L,-1);
			cout<<a<<endl;
			lua_pop(L, 1);
		}
	}
	lua_pop(L,1);
	cout<<"stack:"<<lua_gettop(L)<<endl;
	return 0;
}

int main()
{

	lua_State *L=luaL_newstate();
	luaL_openlibs(L);

	lua_register(L,"ccprint",L_ccprint);

	int ret=luaL_loadfile(L,"test.lua");
	if(ret!=0)
	{
		const char* str=lua_tostring(L,-1);
		if(str)
		{
			cout<<str<<endl;
		}
		return 0;
	}
	lua_pcall(L,0,0,0);
	lua_close(L);
	return 0;
}
