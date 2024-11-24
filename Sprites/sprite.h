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
using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class Grid;

class Sprite {
	Vector2 Dir{ 1,0 };
	Vector2 Origin{ 0,0 };
	Vector2 Scale{ 1,1 };
	float depth=0; //for sorting
	float animSpeed = 0.8f; //lower value = faster animation
	float Rotation = 0;
	RECT spriteRect={ 0,0,0,0 };
	bool isAnim = false;
	RECT frameSize = { 0,0,0,0 };
	int totalFrames=0;
	int frameCount=0;
	float animTime = 0; 
	XMVECTOR colour = Colours::White;
	string spriteName="";//this is used for finding the texture within the texcache
	string filePath="";
	ID3D11ShaderResourceView* texture=nullptr;
	RECT dim{ 0,0,0,0 };
	friend class Creature;

public:
	enum spriteTYPE
	{
		CREATURE = 0, UI = 1
	};
	spriteTYPE type = spriteTYPE::CREATURE;
	Vector2 previousGridPos = Vector2(0, 0);
	bool active = true;
	Vector2 Position{ 400,400 };
	void Init(Vector2 position,Vector2 scale,Vector2 origin);
	void Init(Grid& grid,Vector2 position, Vector2 scale, bool centerOrigin, RECT spriteRect,RECT framerect,int totalframes, float animspeed);
	void Init(Vector2 position, Vector2 scale, Vector2 origin, RECT spriterect);
	void Update(float dTime);
	void Render(SpriteBatch* Batch);
	Sprite(string spriteName, string path, MyD3D& d3d);
	Sprite();
	void setTex(ID3D11ShaderResourceView& tex, const RECT& texRect);
	void setPos(Vector2& pos) {
		Position = pos;
	}
	bool setGridPosition(Grid& grid,int x, int y, bool checkCol=true);
	void setSpriteRect(RECT& SpriteRect) {
		spriteRect = SpriteRect;
		dim = (RECT{long(spriteRect.left * Scale.x), long(spriteRect.top * Scale.y), long(spriteRect.right * Scale.x), long(spriteRect.bottom * Scale.y)});
	}
	RECT getDim() {
		dim = (RECT{ long(spriteRect.left * Scale.x), long(spriteRect.top * Scale.y), long(spriteRect.right * Scale.x), long(spriteRect.bottom * Scale.y) });
		return dim;
	}
	const RECT getspriteRect() {
		return spriteRect;
	}
	void setColour(XMVECTOR newCol) {
		colour = newCol;
	}
	void setScale(Vector2 newScale) {
		Scale = newScale;
	}
};
