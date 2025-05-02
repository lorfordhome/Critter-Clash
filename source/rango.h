#pragma once
#include "raylib-cpp.hpp"
#include "TexCache.h"
#include "ResourceManager.h"

//ENGINE - manages windows, textures
class Rango {
public:
	Rango::Rango() {
		pixelFont.Load("data/LowresPixel-Regular.ttf");
	}
	TextureManager& GetManager() { return resourceManager; }
	raylib::Font& GetFont() {
		return pixelFont;
	}
	float GetFontSizeSmall() {
		return fontSizeSmall;
	}
	float GetFontSizeLarge() {
		return fontSizeLarge;
	}
	bool windowShouldClose=false;
	static void DrawTextBoxed(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);
	static void DrawTextBoxedSelectable(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);
private:
	TextureManager resourceManager;
	raylib::Font pixelFont;
	float fontSizeSmall = 20.f;
	float fontSizeLarge = 50.f;
};