#include <windows.h>
#include <string>
#include <cassert>
#include <d3d11.h>
#include <iomanip>
#include <vector>

#include "WindowUtils.h"
#include "D3DUtil.h"
#include "D3D.h"
#include "SimpleMath.h"
#include "SpriteFont.h"
#include "DDSTextureLoader.h"
#include "CommonStates.h"
#include "sprite.h"
#include "game.h"
#include "Input.h"
#include "LuaHelper.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

const int ASCII_ESC = 27;


//messages come from windows all the time, should we respond to any specific ones?
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//do something game specific here
	switch (msg)
	{
	// respond to a keyboard event
	case WM_CHAR:
		switch (wParam)
		{
		case ASCII_ESC:
		case 'q':
		case 'Q':
			PostQuitMessage(0);
			break;
		case 'L':
		case 'l':
			Game::Get().ApplyLua();
			break;
		case 'G':
		case 'g':
			Game::Get().CreateEnemyGroup();
			break;
		case 'W':
		case 'w':
			Game::Get().ApplyLuaCheats(false);
		case 'D':
		case 'd':
			Game::Get().ApplyLuaCheats(true);
		}
		break;
	case WM_INPUT:
		Game::Get().mouse.MessageEvent((HRAWINPUT)lParam);
		break;
	}

	//default message handling (resize window, full screen, etc)
	return WinUtil::Get().DefaultMssgHandler(hwnd, msg, wParam, lParam);
}

void WinUtil::ResizeWindow(int width, int height) {
	SetWindowPos(mWinData.hMainWnd, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
}

//main entry point for the game
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{

	int w(1024), h(768);
	if (!WinUtil::Get().InitMainWindow(w, h, hInstance, "Critter Clash", MainWndProc, true))
		assert(false);

	MyD3D d3d;
	if (!d3d.InitDirect3D())
		assert(false);
	WinUtil::Get().SetD3D(d3d);
	new Game (d3d);
	bool canUpdateRender;
	float dTime = 0;

	while (WinUtil::Get().BeginLoop(canUpdateRender))
	{
		if (canUpdateRender)
		{
			Game::Get().Update(dTime);
			Game::Get().Render(dTime);
		}
		dTime = WinUtil::Get().EndLoop(canUpdateRender);
	}

	#ifdef _DEBUG
	d3d.ReleaseD3D(true);	
	#else
	d3d.ReleaseD3D(false);
	#endif
	delete& Game::Get();
	return 0;
}