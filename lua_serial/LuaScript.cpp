#include "LuaScript.h"
#include "util/MyStream.h"
#include <string>

using namespace std;

CLuaScript::CLuaScript()
{
	Init();
}

CLuaScript::~CLuaScript()
{
	if(L)
	{
		lua_close(L);
		L=NULL;
	}
}

void CLuaScript::Init()
{
	L=luaL_newstate();
	luaL_openlibs(L);

	REG_LUA_FUNCTION(LoadTable);
	REG_LUA_FUNCTION(SaveTable);
}

void CLuaScript::RegisterFunction(const char* funcName,lua_CFunction func)
{
	if(funcName==NULL)
	{
		return;
	}
	lua_register(L,funcName,func);
}


bool CLuaScript::LoadFile(const char *path)
{
	int ret=luaL_loadfile(L,path);
	if(ret!=0)
	{
		return false;
	}	  
	ret=lua_pcall(L,0,0,0);
	if(ret!=0)
	{
		return false;
	}
	return true;
}

int CLuaScript::L_LoadTable(lua_State* L)
{
	if(!lua_istable(L,1))
	  return 0;
	const char* fileName=lua_tostring(L,2);
	if(fileName==0)
	{
		return 0;
	}
	string file=fileName;
	lua_pop(L,1);
	if(!lua_istable(L,1))
	  return 0;
	int result=0;
	FILE *pf=::fopen(file.c_str(),"rb");
	if(pf!=0)
	{
		long bufferSize=0;
		fseek(pf,0,SEEK_END);
		bufferSize=ftell(pf);
		fseek(pf,0,SEEK_SET);
		uint8* buffer=new uint8[bufferSize+1];
		buffer[0]=0;
		buffer[bufferSize]=0;
		if(fread(buffer,bufferSize,1,pf)!=1)
		{
			::fclose(pf);
			delete [] buffer;
			return 0;
		}
		::fclose(pf);
		if(LoadTableFromData(L,buffer,bufferSize))
		{
			result=1;
		}
		delete [] buffer;
	}
	else
	{
		return 0;
	}
	lua_pushboolean(L,result);
	return 1;

}

bool CLuaScript::LoadTableFromData(lua_State*L,uint8 *data,uint dataSize)
{
	int oldStackPos=lua_gettop(L);
	Stream reader(data,dataSize);
	bool iskey=true;
	bool loadOK=false;
	while(reader.GetSpace()>0)
	{
		if(!LoadValue(L,lua_gettop(L),reader,iskey,loadOK))
		{
			lua_settop(L,oldStackPos);
			return false;
		}
		if(loadOK)
		  break;
	}
	if(lua_gettop(L)!=oldStackPos)
	{
		lua_settop(L,oldStackPos);
		return false;
	}
	return true;
}

bool CLuaScript::LoadValue(lua_State* L, int idx, BaseStream& reader, bool& isKey, bool& loadOk)
{
	if (reader.GetSpace() < 1)
		return true;
	uint8 type = 0;
	reader >> type;
	switch (type)
	{
	case EScriptStreamType_number:
	{
		double doubleValue;
		reader >> doubleValue;
		lua_pushnumber(L, doubleValue);
		//如果上一层不是table就一定是key
		//-1 number value
		//-2 key
		//-3 table
		if (!isKey&&lua_istable(L, -3))
		{
			if (lua_type(L, -2) == LUA_TNUMBER || lua_type(L, -2) == LUA_TSTRING)
				lua_settable(L, -3);
		}
		isKey = !isKey;
	}
		break;
	case EScriptStreamType_string:
	{
		string strValue;
		reader >> strValue;
		lua_pushstring(L, strValue.c_str());
		//-1 string value
		//-2 key
		//-3 table
		if (!isKey&&lua_istable(L, -3))
		{
			if (lua_type(L, -2) == LUA_TNUMBER || lua_type(L, -2) == LUA_TSTRING)
				lua_settable(L, -3);
		}
		isKey = !isKey;
	}
		break;
	case EScriptStreamType_bool:
		{
			uint8 b = 0;
			reader >> b;
			lua_pushboolean(L, b);
			//-1 string value
			//-2 key
			//-3 table
			if (!isKey&&lua_istable(L, -3))
			{
				if (lua_type(L, -2) == LUA_TNUMBER || lua_type(L, -2) == LUA_TSTRING)
					lua_settable(L, -3);
			}
			isKey = !isKey;
		}
		break;
	case EScriptStreamType_table:
		{
			uint8 flagValue = 0;
			reader >> flagValue;
			if (flagValue == EScriptStreamType_end)//整个数据块的结束标记
			{
				loadOk = true;
				return isKey;
			}
			else if (flagValue == EScriptStreamTableFlag_Begin)//table 开始标记
			{
				lua_newtable(L);
				//-1 sub table
				//-2 key
				//-3 parent table
				Assert(lua_istable(L, -3));
				Assert(lua_type(L, -2) == LUA_TNUMBER || lua_type(L, -2) == LUA_TSTRING);
				Assert(!isKey);
				if (!isKey && lua_istable(L, -3))
				{
					Assert(lua_type(L, -2) == LUA_TNUMBER || lua_type(L, -2) == LUA_TSTRING);
				}
				isKey = !isKey;
				bool loadOk2 = false;
				//加载子表
				if (!LoadValue(L, lua_gettop(L), reader, isKey, loadOk2))
					return false;
			}
			else if (flagValue == EScriptStreamTableFlag_End)
			{
				//现在最上面一层一定是table
				//-1 sub table
				//-2 key
				//-3 parent table
				Assert(lua_istable(L, -1) && lua_istable(L, -3));
				if (lua_istable(L, -1))
				{
					Assert(lua_type(L, -2) == LUA_TNUMBER || lua_type(L, -2) == LUA_TSTRING);
					if (lua_type(L, -2) == LUA_TNUMBER || lua_type(L, -2) == LUA_TSTRING)
						lua_settable(L, -3);
				}
			}
			else
			{
				Assert(false);
				return false;
			}
		}
		break;
	case EScriptStreamType_end:
		{
			Assert(isKey);
			loadOk = true;
			return isKey;
		}
		break;
	default:
		Assert(false);
		return false;
	}
	return true;
}

int CLuaScript::L_SaveTable(lua_State* L)
{
	if (lua_gettop(L) != 2)
		return 0;
	if (!lua_istable(L, 1))
		return 0;
	const char* fileName = lua_tostring(L, 2);
	if (fileName == 0)
		return 0;
	string file = fileName;
	lua_pop(L, 1);
	if (!lua_istable(L, -1))
		return 0;

	int oldStackPos = lua_gettop(L);
	int saveTableSize = 0;
	if (!GetSaveTableSize(L, lua_gettop(L), saveTableSize, true))
	{
		lua_settop(L, oldStackPos);
		return 0;
	}
	uint8* saveBuffer = new uint8[saveTableSize];
	uint writeSize = 0;

	if (!SaveTableToData(L, saveBuffer, saveTableSize, writeSize))
	{
		SAFE_DELETE_ARRAY(saveBuffer);
		return 0;
	}

	//流数据保存到文件
	//如果文件存在会被清空
	FILE* pf = ::fopen(file.c_str(), "wb+");
	if (pf != 0)
	{
		if (writeSize > 0)
		{
			::fwrite(saveBuffer, 1, writeSize, pf);
		}
		::fclose(pf);
		SAFE_DELETE_ARRAY(saveBuffer);
		lua_pushnumber(L, writeSize);
		return 1;
	}
	else
	{
		SAFE_DELETE_ARRAY(saveBuffer);
	}
	return 0;
}

bool CLuaScript::GetSaveTableSize(lua_State* L, int idx, int& saveSize, bool writeEndFlag)
{
	int oldStackPos = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L,idx))
	{
		int keyType = lua_type(L, -2);
		int valueType = lua_type(L, -1);
		if (keyType == LUA_TBOOLEAN || keyType == LUA_TNUMBER || keyType == LUA_TSTRING)
		{
			if (valueType == LUA_TBOOLEAN || valueType == LUA_TNUMBER || valueType == LUA_TSTRING || valueType == LUA_TTABLE)
			{
				if (!GetSaveValueSize(L, -2,saveSize))
					return false;
				if (!GetSaveValueSize(L, -1, saveSize))
					return false;
				//子表
				if (lua_istable(L, -1))
				{
					if (!GetSaveTableSize(L, lua_gettop(L), saveSize, false))
					{
						lua_settop(L, oldStackPos);
						return false;
					}
					saveSize += 2;
				}
			}
		}
		lua_pop(L, 1);//因为lua_next是pop1个，push2个所以这儿只需要pop1个就可以了
	}
	if (lua_gettop(L) != oldStackPos)
	{
		return false;
	}
	if (writeEndFlag)
		++saveSize;
	return true;
}

bool CLuaScript::GetSaveValueSize(lua_State* L, int idx, int& saveSize)
{
	int type = lua_type(L, idx);
	switch (type)
	{
	case LUA_TSTRING:
		{
			const char* s = lua_tostring(L, idx);
			if (!s)
			{
				return false;
			}
			uint size = (uint)::strlen(s);
			saveSize += (1 + sizeof(uint)+size);
		}
		break;
	case LUA_TNUMBER:
		{
			saveSize += (1 + sizeof(lua_Number));
		}
		break;
	case LUA_TBOOLEAN:
		 saveSize += 2;
		break;
	case LUA_TTABLE:
		{
			Assert(idx == -1);//table只能是value 绝不可能是key
			if (idx != -1)
			   return false;
			saveSize+=2;
		}
		break;
	default:
		break;
	}
	return true;
}

bool CLuaScript::SaveTableToData(lua_State* L, uint8* data, uint dataSize, uint& writeSize)
{
	int oldStackPos = lua_gettop(L);
	Stream writer(data, dataSize);
	if (!SaveTable(L, lua_gettop(L), writer, true))
	{
		lua_settop(L,oldStackPos);
		return false;
	}
	if (lua_gettop(L) != oldStackPos)
	{
		return lua_settop(L, oldStackPos), false;
	}
	writeSize = (uint)writer.GetOffset();
	return true;
}

bool CLuaScript::SaveTable(lua_State* L, int idx, BaseStream& writer, bool writeEndFlag)
{
	int oldStackPos = lua_gettop(L);

	lua_pushnil(L);
	while (lua_next(L,idx))
	{
		int keyType = lua_type(L, -2);
		int valueType = lua_type(L, -1);
		if (keyType == LUA_TBOOLEAN || keyType == LUA_TNUMBER || keyType == LUA_TSTRING)
		{
			if (valueType == LUA_TBOOLEAN || valueType == LUA_TNUMBER || valueType == LUA_TSTRING || valueType == LUA_TTABLE)
			{
				if (!SaveValue(L, -2, writer))
					return false;
				if (!SaveValue(L, -1, writer))
					return false;
				//子表
				if (lua_istable(L, -1))
				{
					if (!SaveTable(L, lua_gettop(L), writer, false))
					{
						lua_settop(L, oldStackPos);
						return false;
					}
					writer << ((uint8)EScriptStreamType_table);
					writer << ((uint8)EScriptStreamTableFlag_End);
				}
			}
		}
		lua_pop(L, 1);//因为lua_next是pop1个，push2个所以这儿只需要pop1个就可以了
	}
	if (lua_gettop(L) != oldStackPos)
	{
		lua_settop(L, oldStackPos);
		return false;
	}
	if (writeEndFlag)
		writer << ((uint8)EScriptStreamTableFlag_End);
	return true;
}

bool CLuaScript::SaveValue(lua_State* L, int idx, BaseStream& writer)
{
	int type = lua_type(L, idx);
	switch (type)
	{
	case LUA_TSTRING:
		{
			const char* s = lua_tostring(L, idx);
			Assert(s);
			if (!s)
			{
				return false;
			}
			writer << ((uint8)EScriptStreamType_string);
			writer << (s);
		}
		break;
	case LUA_TNUMBER:
		{
			writer<<((uint8)EScriptStreamType_number);
			writer << (lua_tonumber(L, idx));
		}
		break;
	case LUA_TBOOLEAN:
		{
			writer<< ((uint8)EScriptStreamType_bool);
			writer << (lua_toboolean(L, idx) == 0 ? false : true);
		}
		break;
	case LUA_TTABLE:
		{
			Assert(idx == -1);//table只能是value 绝不可能是key
			if (idx != -1)
				return false;
			writer << ((uint8)EScriptStreamType_table);
			writer << ((uint8)EScriptStreamTableFlag_Begin);
		}
		break;
	default:
		break;
	}
	return true;
}












































