#pragma once

#include <string>
#include <functional>
#include <map>

extern "C" {
#include "../lua-5.4.4/include/lua.h"
#include "../lua-5.4.4/include/lauxlib.h"
#include "../lua-5.4.4/include/lualib.h"
}

#ifdef _WIN32
#pragma comment(lib,"../lua-5.4.4/liblua54.a")
#endif
