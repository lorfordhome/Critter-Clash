#pragma once
#include "raylib-cpp.hpp"
#include "TexCache.h"
#include "ResourceManager.h"

//ENGINE - manages windows, textures
class Rango {
public:
	Rango::Rango() {}
	TextureManager& GetManager() { return resourceManager; }
	bool windowShouldClose=false;
private:
	TextureManager resourceManager;
};