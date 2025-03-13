#include <assert.h>
#include <iostream>
#include "LuaHelper.h"
using namespace std;

bool LuaOK(lua_State* L, int id) {
	if (id != LUA_OK) {
		cout << lua_tostring(L, -1) << endl; //print out error
		return false;
	}
	return true;
}

int LuaGetInt(lua_State* L, const string& name) {
	lua_getglobal(L, name.c_str());
	if (!lua_isinteger(L, -1))
		assert(false);
	return (int)lua_tointeger(L, -1);
}

string LuaGetStr(lua_State* L, const string& name) {
	lua_getglobal(L, name.c_str());
	if (!lua_isstring(L, -1))
		assert(false);
	return lua_tostring(L, -1);
}

int GetType(lua_State* L, const std::string& name)
{
	
	lua_getglobal(L, name.c_str());
	if (!lua_istable(L, -1))
		assert(false);
	lua_pushstring(L, "type");
	lua_gettable(L, -2); //pops x off and replaces it with  the value
	int Type = (int)lua_tointeger(L, -1);
	//lua_pop(L, 1);
	lua_settop(L, 0);
	return Type;
}