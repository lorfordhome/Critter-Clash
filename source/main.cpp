#include <string>
#include <cassert>
#include <iomanip>
#include <vector>
#include "sprite.h"
#include "game.h"
#include "Input.h"
#include "LuaHelper.h"
#include "raylib-cpp.hpp"

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

	int w(1024), h(768);
	raylib::Window window(w, h, "Critter Clash");
	float deltaTime = 0;
	Rango rango;
	new Game(rango);

	while (!window.ShouldClose())
	{
		deltaTime = GetFrameTime();
		//update
		Game::Get().Update(deltaTime);

		//render
		Game::Get().Render(deltaTime);
	}
	delete& Game::Get();
	return 0;
}