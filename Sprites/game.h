#pragma once
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

class Game {
	enum class State{START,PLAY,END};
	State state = State::START;

	std::vector<Sprite> gameSprites;
public:
	Game(MyD3D& md3d);
	void Release();
	void Update(float dTime);
	void Render(float dTime);
private:
	MyD3D& md3d;
	SpriteBatch* mySpriteBatch = nullptr;
};