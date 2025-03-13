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


struct EnemyDetails {
	Vector2L position;
	creatureType type;
};

//check for errors
bool LuaOK(lua_State* L, int id);
//get int from lua by name
int LuaGetInt(lua_State* L, const std::string& name);
//get string from lua by name
std::string LuaGetStr(lua_State* L, const std::string& name);
int GetType(lua_State* L, const std::string& name);