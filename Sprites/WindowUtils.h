#ifndef WINDOWSUTILS
#define WINDOWSUTILS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <sstream>
#include <assert.h>

class MyD3D;

/*
Manage the Microsoft Windows side of the game app
*/
class WinUtil
{
private:
	struct WinData
	{
		//all windows apps have these handles
		HINSTANCE hAppInst = 0;
		HWND      hMainWnd = 0;
		bool      appPaused = false;
		bool      minimized = false;
		bool      maximized = false;
		bool      resizing = false;
		std::string mainWndCaption;
		int clientWidth;
		int clientHeight;
		const int minClientWidth = 200;
		const int minClientHeight = 200;
	};
	WinData mWinData;
	MyD3D *mpMyD3D;

	//clients cannot call this
	WinUtil()
		:mpMyD3D(nullptr)
	{}

public:
	//we mustn't haev any of these
	WinUtil(WinUtil const&) = delete;
	void operator=(WinUtil const&) = delete;
	
	static WinUtil& Get()
	{ 
		static WinUtil instance; 
		return instance;
	}
	//setup a window to render into
	bool InitMainWindow(int width, int height, HINSTANCE hInstance, const std::string& appName, WNDPROC mssgHandler, bool centred=true);
	//make the link between windows and D3D
	void SetD3D(MyD3D& d3d) {
		assert(mpMyD3D == nullptr);
		mpMyD3D = &d3d;
	}
	/*
	Call this before your Update/Render, and only call Update/Render
	if the bool canUpdateRender is true.
	Returns false if windows is asking us to quit.
	*/
	bool BeginLoop(bool& canUpdateRender);
	/*
	Call this after your Update/Render.
	Save that canUpdateRender variable you used earlier and pass it through.
	Return the elapsed time for this update in seconds (so it will usually be a very small number)
	*/
	float EndLoop(bool didUpdateRender);
	//handle messages from the operating system
	LRESULT DefaultMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	const WinData& GetData() const {
		return mWinData;
	}
	void ChooseRes(int& w, int& h, int defaults[], int numPairs);

};

#endif
