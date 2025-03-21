#pragma once

#include <string>
#include <functional>
#include <map>
#include "creature.h"

extern "C" {
#include "../lua-5.4.4/include/lua.h"
#include "../lua-5.4.4/include/lauxlib.h"
#include "../lua-5.4.4/include/lualib.h"
}

#ifdef _WIN32
#pragma comment(lib,"../lua-5.4.4/liblua54.a")
#endif

struct Vector2L {
	float x, y;

	void Fromlua(lua_State* L, const std::string& name) {
		lua_getglobal(L, name.c_str());
		if (!lua_istable(L, -1))
			assert(false);
		lua_pushstring(L, "x");
		lua_gettable(L, -2); //pops x off and replaces it with  the value
		x = lua_tonumber(L, -1); //grab the value associated with the pair, assign it to the struct value
		lua_pop(L, 1); //clean up

		lua_pushstring(L, "y");
		lua_gettable(L, -2); //pops x off and replaces it with  the value
		y = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
};

struct creatureDetails {
	Vector2 position;
	creatureType type;

	void fromLua(lua_State* L, int subTable, int creatureIndex) {
		lua_getglobal(L, "Troops");  // Push the table "Troops" onto the stack

		if (!lua_istable(L, -1))
			assert(false);

		
		lua_rawgeti(L, -1,subTable);    // Get Troops[subTable]
		if (!lua_istable(L, -1))
			assert(false);

		lua_rawgeti(L, -1, creatureIndex);   // Get Troops[subTable][creatureIndex]

		if (!lua_istable(L, -1))
			assert(false);

		//retrieve pos
		lua_pushstring(L, "x");
		lua_gettable(L, -2); //pops x off and replaces it with  the value
		position.x = lua_tonumber(L, -1); //grab the value associated with the pair, assign it to the struct value
		lua_pop(L, 1); //clean up

		lua_pushstring(L, "y");
		lua_gettable(L, -2); //pops x off and replaces it with  the value
		position.y = lua_tonumber(L, -1);
		lua_pop(L, 1);

		//retrieve type
		lua_pushstring(L, "type");
		lua_gettable(L, -2); //pops  off and replaces it with  the value
		type = static_cast<creatureType>(lua_tointeger(L, -1));
		lua_pop(L, 1);

		lua_pop(L, 2);  // Remove Troops[1][1], Troops[1], and Troops
	}
};



//check for errors
bool LuaOK(lua_State* L, int id);
//get int from lua by name
int LuaGetInt(lua_State* L, const std::string& name);
//get string from lua by name
std::string LuaGetStr(lua_State* L, const std::string& name);
int GetType(lua_State* L, const std::string& name);
void CallWriteTroops(lua_State* L, int difficulty, const char* stringToWrite);
void Execute(lua_State* L, std::string szScript);
void CallFunction(lua_State* L, std::string szName);
void Error(lua_State* L, int nStatus);
int GetTableLength(lua_State* L, const char* TableToCheck);
int Get2DTableLength(lua_State* L, const char* tableToCheck, int subTableIndex);