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
	lua_pop(L, 1);
	return Type;
}

void CallWriteTroops(lua_State* L, int difficulty, const char* stringToWrite) {
	lua_getglobal(L, "WriteTroops");
	if (!lua_isfunction(L, -1))
		assert(false);
	lua_pushinteger(L, difficulty);
	lua_pushstring(L, stringToWrite);
	if (!LuaOK(L, lua_pcall(L, 2, 0, 0)))
		assert(false);
}

int GetTableLength(lua_State* L, const char* tableToCheck) {
	lua_getglobal(L, tableToCheck);
	if (!lua_istable(L, -1))
		assert(false);
	lua_len(L, -1);
	int length = lua_tointeger(L, -1);
	lua_pop(L, 2); //remove both the table and the length value off the stack
	return length;
}

int Get2DTableLength(lua_State* L, const char* tableToCheck, int subTableIndex) {
	lua_getglobal(L, tableToCheck);  // Push the table "Troops" onto the stack

	if (!lua_istable(L, -1))
		assert(false);

	lua_pushinteger(L, subTableIndex);  // Push index
	lua_gettable(L, -2);    // Get Troops[subTable]
	if (!lua_istable(L, -1))
		assert(false);
	lua_len(L, -1);
	int length = lua_tointeger(L, -1);

	lua_pop(L, 3); //remove both tables and the length value off the stack
	return length;

}

void Execute(lua_State* L, std::string szScript)
{
	int nStatus = 0;

	nStatus = luaL_loadfile(L, szScript.c_str());
	if (nStatus == 0) { nStatus = lua_pcall(L, 0, LUA_MULTRET, 0); }

	Error(L, nStatus);
}

void CallFunction(lua_State* L, std::string szName)
{
	int nStatus = 0;

	lua_getglobal(L, szName.c_str());
	nStatus = lua_pcall(L, 0, LUA_MULTRET, 0);

	Error(L,nStatus);
}

void Error(lua_State* L, int nStatus)
{
	if (nStatus != 0)
	{
		std::string szError = lua_tostring(L, -1);
		szError = "LUA:\n" + szError;
		MessageBox(NULL, szError.c_str(), "Error", MB_OK | MB_ICONERROR);
		lua_pop(L, 1);
	}
}