#pragma once
#include "raylib-cpp.hpp"
#include "ResourceManager.h"
#include "raymath.hpp"

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

//ENGINE - manages windows, textures
class Rango {
public:
	Rango::Rango() {
		pixelFont.Load("data/LowresPixel-Regular.ttf");
		UpdateWindowSize();
	}
	//getters
	TextureManager& GetManager() { return resourceManager; }
	raylib::Font& GetFont() {return pixelFont;}
	float GetFontSizeSmall() const {return fontSizeSmall;}
	float GetFontSizeLarge() const {return fontSizeLarge;}
	Vector2 GetGameScreenSize() const { return screenSize; }
	int GetGameScreenWidth() const {return screenSize.x;}
	int GetGameScreenHeight() const {return screenSize.y;}
	float GetScreenScale() const { return screenScale; }
	Vector2 GetWindowSize() const { return windowSize; }
	//functions
	static void DrawTextBoxed(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);
	static void DrawTextBoxedSelectable(Font font, const char* text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);
	void UpdateWindowSize();
	//members
private:
	TextureManager resourceManager;
	raylib::Font pixelFont;
	const Vector2 screenSize = { 640,360 };
	Vector2 windowSize = { 0,0 };
	float fontSizeSmall = 10.f;
	float fontSizeLarge = 25.f;
	float screenScale = 1;
public:
	bool windowShouldClose = false;
};