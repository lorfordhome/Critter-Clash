#include <math.h>
#include <cassert>
#include <sstream>

#include"game.h"
#include "Input.h"
#include "D3D.h"
#include "D3DUtil.h"
#include "WindowUtils.h"
#include <algorithm> //for copying array
using namespace DirectX;
using namespace DirectX::SimpleMath;

// get the absolute mouse position, not raw input, just standard windows is fine
void Mouse::GetMousePosAbsolute(Vector2& pos)
{
	POINT mpos;
	if (GetCursorPos(&mpos))
	{
		if (ScreenToClient(mHwnd, &mpos))
		{
			pos = Vector2((float)mpos.x, (float)mpos.y);
		}
	}
}

void Mouse::Initialise(HWND hwnd, bool showMouse, bool confineMouse)
{
	mHwnd = hwnd;

	RAWINPUTDEVICE Rid[2];

	//these ids trigger mouse input
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x02;
	Rid[0].dwFlags = RIDEV_INPUTSINK;// RIDEV_NOLEGACY;   // adds HID mouse and also ignores legacy mouse messages
	Rid[0].hwndTarget = hwnd;


	// RIDEV_NOLEGACY will stop normal message pump WM_CHAR type messages
	// sometimes we might want that, for now we'll leave it, then we can
	// still use the normal message pump for things like player name entry
	if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE) {
		MessageBox(0, "Cannot get keyboard and mouse input", 0, 0);
		assert(false);
	}


	ShowCursor(showMouse);
	mConfineMouse = confineMouse;
	GetClipCursor(&mOldClip);
	if (mConfineMouse)
	{
		GetWindowRect(hwnd, &mNewClip);
		ClipCursor(&mNewClip);
	}
	GetMousePosAbsolute(mMouseScreen);
	Reset();
}

void Mouse::ProcessMouse(RAWINPUT* raw)
{
	//store value of mouse buttons
	copy(mButtons, mButtons + MAX_BUTTONS, lastmButtons);
	unsigned short flags = raw->data.mouse.usButtonFlags;

	if (flags & RI_MOUSE_LEFT_BUTTON_DOWN)
		mButtons[LBUTTON] = true;
	if (flags & RI_MOUSE_LEFT_BUTTON_UP)
		mButtons[LBUTTON] = false;
	if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
		mButtons[MBUTTON] = true;
	if (flags & RI_MOUSE_MIDDLE_BUTTON_UP)
		mButtons[MBUTTON] = false;
	if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN)
		mButtons[RBUTTON] = true;
	if (flags & RI_MOUSE_RIGHT_BUTTON_UP)
		mButtons[RBUTTON] = false;

	Vector2 last(mMouseScreen);
	GetMousePosAbsolute(mMouseScreen);
	mMouseMove = mMouseScreen - last;
}

bool Mouse::isClickRelease() {
	return (mButtons[LBUTTON] == false && lastmButtons[LBUTTON] == true);
}
void Mouse::Reset()
{
	ZeroMemory(mInBuffer, sizeof(mInBuffer));
	mButtons[0] = mButtons[1] = mButtons[2] = false;
	mMouseScreen = mMouseMove = DirectX::SimpleMath::Vector2(0, 0);
}

void Mouse::MessageEvent(HRAWINPUT rawInput)
{
	UINT dwSize;

	UINT res = GetRawInputData(rawInput, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	if (res != 0)
		return;//bad input so ignore it

	if (dwSize >= RAWBUFF_SIZE)
	{
		DBOUT("Buffer too small. Is " << RAWBUFF_SIZE << " wants " << dwSize);
		assert(false);
	}

	if (GetRawInputData(rawInput, RID_INPUT, mInBuffer, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
	{
		DBOUT("GetRawInputData failed");
		assert(false);
	}

	RAWINPUT* raw = (RAWINPUT*)mInBuffer;

	if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		ProcessMouse(raw);
	}
}

void Mouse::OnLost()
{
	Reset();
	ClipCursor(&mOldClip);
}

void Mouse::OnReset()
{
	Reset();
	ClipCursor(&mNewClip);
}

void Mouse::PostProcess()
{
	mMouseMove.x = 0;
	mMouseMove.y = 0;
}



