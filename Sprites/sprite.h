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

public:
	Vector2 previousGridPos = Vector2(0, 0);
	bool isMovingSprite = false;
	Vector2 Position{ 400,400 };
	void Init(Vector2 position,Vector2 scale,Vector2 origin);
	void Init(Grid& grid,Vector2 position, Vector2 scale, bool centerOrigin, RECT spriteRect,RECT framerect,int totalframes, float animspeed);
	void Update(float dTime, MyD3D& d3d);
	void Render(MyD3D& d3d, SpriteBatch* Batch);
	Sprite(string spriteName, string path, MyD3D& d3d);
	Sprite();
	void setTex(ID3D11ShaderResourceView& tex, const RECT& texRect);
	void setPos(Vector2& pos) {
		Position = pos;
	}
	void setGridPosition(Grid& grid,int x, int y);
	Vector2 getGridPosition(Grid& grid);
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
};
