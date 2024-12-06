#ifndef INPUT_H
#define INPUT_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#include <string>

#include "D3D.h"
#include "SimpleMath.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;
class Grid;
class Mouse{
public:
	bool clickedLastFrame = false;
	void GetMousePosAbsolute(Vector2& pos);
	void ProcessMouse(RAWINPUT* raw);

	//returns true if LMOUSE has just been released
	bool isClickRelease();

	//zero and forget everything so we can start again
	void Reset();

	//add to your windows message pump
	//rawInput comes from lparam after a WM_INPUT message
	//case WM_INPUT:
	//	input.MessageEvent((HRAWINPUT)lParam);
	void MessageEvent(HRAWINPUT rawInput);

	//for consistency we can reset mouse input after task switch (window<->fullscreen), optional
	void OnLost();
	void OnReset();

	//call this after game has finished using the mouse, probably at end of render
	void PostProcess();

	//start up the mouse/keys system, call once only
	//hwnd - main windows handle
	//showMouse - hide the mouse cursor
	//confineMouse - prevent the mouse leaving the window
	void Initialise(HWND hwnd, bool showMouse, bool confineMouse);

	//absolute=true, 2D windows coordinates of cursor
	//absolute=false, 2D windows coordinate change in position since last update (push)
	DirectX::SimpleMath::Vector2 GetMousePos(bool absolute) const {
		if (absolute)
			return mMouseScreen;
		return mMouseMove;
	}

	//check if a button is currently down
	typedef enum { LBUTTON = 0, MBUTTON = 1, RBUTTON = 2 } ButtonT;
	bool GetMouseButton(ButtonT b) const {
		return mButtons[b];
	}


private:

	enum { RAWBUFF_SIZE = 512, KEYBUFF_SIZE = 255, KMASK_IS_DOWN = 1, MAX_BUTTONS = 3 };
	//raw input buffer
	BYTE mInBuffer[RAWBUFF_SIZE];
	//copy of main window handle
	HWND mHwnd;
	//track mouse position changes (2D)
	DirectX::SimpleMath::Vector2 mMouseScreen, mMouseMove;
	//if the mouse is confined, track the size of the window
	bool mConfineMouse;
	RECT mNewClip;           // new area for ClipCursor
	RECT mOldClip;        // previous area for ClipCursor
	//support for three mouse buttons
	bool mButtons[MAX_BUTTONS];
	bool lastmButtons[MAX_BUTTONS];
};


#endif