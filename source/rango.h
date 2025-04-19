#pragma once
#include "raylib-cpp.hpp"
#include "TexCache.h"

//ENGINE - manages windows, textures
class Rango {
public:
	TexCache& GetCache() { return mTexCache; }
private:
	TexCache mTexCache;
};