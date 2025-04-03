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

class Dispatcher {
public:
	//capture game functions here
	struct Command {
		typedef std::function<void(int)> voidintfunc; //created sig for a function which returns nothing and takes an int
		voidintfunc voidintfunct;
		//add any other function signatures here
		typedef std::function<void(void)> voidvoidfunc; //created sig for a void func that takes nothing
		voidvoidfunc voidvoidfunct;
	};
	//call once at start
	void Init(lua_State* L) {
		lua_register(L, "CDispatcher", LuaCall);
	}
	//register game functions
	void Register(const std::string& name, Command cmd) {
		assert(library.find(name) == library.end());
			library[name] = cmd;
	}
	//lua calls this then the data gets dispatched to the named function
	//lua is C based so cannot call class member function swithout help
	static int LuaCall(lua_State* L);
private:
	static std::map<std::string, Command> library; //where game functions are stored
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
void CallVoidVoidCFunc(lua_State* L, const std::string& fname);