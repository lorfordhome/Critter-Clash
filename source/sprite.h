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
protected:
	Vector2 Dir{ 1,0 };
	Vector2 Origin{ 0,0 };
	Vector2 Scale{ 1,1 };
	float depth=0; //for sorting
	float animSpeed = 0.8f; //lower value = faster animation
	float Rotation = 0;
	RECT spriteRect={ 0,0,0,0 };
	RECT frameSize = { 0,0,0,0 };
	bool isAnim = false;
	unsigned char totalFrames=0;
	unsigned char frameCount=0;
	float animTime = 0; 
	XMVECTOR colour = Colours::White;
	string spriteName="";//this is used for finding the texture within the texcache
	string filePath="";
	ID3D11ShaderResourceView* texture=nullptr;
	RECT dim{ 0,0,0,0 };
	friend class Creature;
public:
	bool isHover = false;
	bool active = true;
	~Sprite() {
		texture = nullptr;
		delete texture;
	}
	enum spriteTYPE
	{
		CREATURE = 0, UI = 1
	};
	spriteTYPE type = spriteTYPE::CREATURE;
	Vector2 previousGridPos = Vector2(0, 0);
	Vector2 Position{ 400,400 };
	void Init(Vector2 position, Vector2 scale, bool centerOrigin, RECT spriteRect,RECT framerect,int totalframes, float animspeed);
	void Init(Vector2 position, Vector2 scale, Vector2 origin, RECT spriterect);
	virtual void Update(float dTime);
	virtual void Render(SpriteBatch* Batch);
	Sprite(string spriteName, string path, MyD3D& d3d);
	Sprite();
	void setTex(ID3D11ShaderResourceView* tex, const RECT& texRect);
	void setTex(ID3D11ShaderResourceView* tex);
	void setPos(Vector2& pos) {
		Position = pos;
	}
	bool setGridPosition(Grid& grid,int x, int y, bool checkCol=true);
	//change grid pos without resetting previous pos to empty
	bool setGridPositionNoPrev(Grid& grid, int x, int y, bool checkCol=true); 
	void setSpriteRect(RECT& SpriteRect) {
		spriteRect = SpriteRect;
	}
	RECT getDim() {
		dim = (RECT{ long(spriteRect.left * Scale.x), long(spriteRect.top * Scale.y), long(spriteRect.right * Scale.x), long(spriteRect.bottom * Scale.y) });
		return dim;
	}
	const RECT getSpriteRect() {
		return spriteRect;
	}
	const RECT getFrameSize() {
		return frameSize;
	}
	void setColour(XMVECTOR newCol) {
		colour = newCol;
	}
	void setScale(Vector2 newScale) {
		Scale = newScale;
	}
	void setAnimValues(int totalframes, float animspeed) {
		totalFrames = totalframes;
		animSpeed = animspeed;
		//reset animation counter
		animTime = 0;
		frameCount = 0;
	}
	void setAnimValues(int totalframes, float animspeed, RECT frameRect) {
		frameSize = frameRect;
		totalFrames = totalframes;
		animSpeed = animspeed;
		//reset animation counter
		animTime = 0;
		frameCount = 0;
	}
	virtual bool Action() {
		return true;
	}
};

class UISprite : public Sprite {
	float clickTimer = 0; //tracking how long until it can be clicked again
public:
	~UISprite() {
		texture = nullptr;
	}
	UISprite(string spriteName, string path, MyD3D& d3d);
	UISprite();
	enum UITYPE {
		none, start, options, next, restart, menu, quit, store,sell
	};
	UITYPE uiType = UITYPE::none;
	float clickCooldown = 0;//how long before button can be clicked again
	bool canBeClicked = true;
	void Update(float dTime) override;
	bool Action() override; //returns true if can be clicked
};
