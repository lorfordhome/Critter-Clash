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

//if ALT+ENTER or resize or drag window we might want do
//something like pause the game perhaps, but we definitely
//need to let D3D know what's happened (OnResize_Default).
void OnResize(int screenWidth, int screenHeight, MyD3D& d3d)
{
	d3d.OnResize_Default(screenWidth, screenHeight);
}

//messages come from windows all the time, should we respond to any specific ones?
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//do something game specific here
	switch (msg)
	{
	// Respond to a keyboard event.
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
		}
		break;
	case WM_INPUT:
		Game::Get().mouse.MessageEvent((HRAWINPUT)lParam);
		break;
	}

	//default message handling (resize window, full screen, etc)
	return WinUtil::Get().DefaultMssgHandler(hwnd, msg, wParam, lParam);
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
	delete& Game::Get();
	#else
	d3d.ReleaseD3D(false);
	#endif
	return 0;
}