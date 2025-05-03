#include <string>
#include <cassert>
#include <iomanip>
#include <vector>
#include "sprite.h"
#include "game.h"
#include "LuaHelper.h"
#include "raylib-cpp.hpp"
#include "rango.h"

using namespace std;

const int ASCII_ESC = 27;


//	case WM_CHAR:
//		switch (wParam)
//		{
//		case ASCII_ESC:
//		case 'q':
//		case 'Q':
//			PostQuitMessage(0);
//			break;
//		case 'L':
//		case 'l':
//			Game::Get().ApplyLua();
//			break;
//		case 'G':
//		case 'g':
//			Game::Get().CreateEnemyGroup();
//			break;
//		case 'W':
//		case 'w':
//			Game::Get().ApplyLuaCheats(false);
//		case 'D':
//		case 'd':
//			Game::Get().ApplyLuaCheats(true);


//main entry point for the game
int main()
{
	Vector2 screenSize = { 640,360 };
	Vector2 windowSize = { 1280,720 };
	raylib::Window window(windowSize.x, windowSize.y, "Critter Clash", FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	window.SetMinSize(screenSize);
	float deltaTime = 0;
	Rango rango;
	new Game(rango);
	SetTargetFPS(60);


	while ( !window.ShouldClose() && !rango.windowShouldClose)
	{
		deltaTime = GetFrameTime();
		//update
		Game::Get().Update(deltaTime);

		//render
		Game::Get().Render(deltaTime);
	}
	CloseWindow();
	delete& Game::Get();
	return 0;
}