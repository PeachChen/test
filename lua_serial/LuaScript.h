#include "util/DefineHeader.h"

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}
#define REG_LUA_FUNCTION(funcName) RegisterFunction(#funcName,L_##funcName)

//脚本序列化标志
enum EScriptStreamType
{
	EScriptStreamType_bool=0x01,
	EScriptStreamType_number=0x02,
	EScriptStreamType_string=0x03,
	EScriptStreamType_table=0x04,
	EScriptStreamType_end=0xFF,	
};

//table保存标记
enum EScriptStreamTableFlag
{
	EScriptStreamTableFlag_Begin=1,
	EScriptStreamTableFlag_End=2,
};

class BaseStream;

class CLuaScript
{
	public:
		SINGLETON_NODE(CLuaScript);
		CLuaScript();
		~CLuaScript();
		void Init();

		bool LoadFile(const char *path);
		static int L_LoadTable(lua_State* L);
		static int L_SaveTable(lua_State* L);
		
		static bool LoadTableFromData(lua_State*L,uint8 *data,uint dataSize);
		static bool LoadValue(lua_State* L, int idx, BaseStream& reader, bool& isKey, bool& loadOk);
		static bool SaveValue(lua_State* L, int idx, BaseStream& writer);
		static bool SaveTable(lua_State* L, int idx, BaseStream& writer, bool writeEndFlag);
		static bool SaveTableToData(lua_State* L, uint8* data, uint dataSize, uint& writeSize);
		static bool GetSaveTableSize(lua_State* L, int idx, int& saveSize, bool writeEndFlag);
		static bool GetSaveValueSize(lua_State* L, int idx, int& saveSize);
	private:
		void RegisterFunction(const char* funcName,lua_CFunction func);
		lua_State* L;
};
