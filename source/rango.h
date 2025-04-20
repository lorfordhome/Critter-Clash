#pragma once
#include "raylib-cpp.hpp"
#include "TexCache.h"

//ENGINE - manages windows, textures
class Rango {
public:
	TexCache& GetCache() { return mTexCache; }
	bool windowShouldClose=false;
private:
	TexCache mTexCache;
};