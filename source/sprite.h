#pragma once
#include <string>
#include <cassert>
#include <iomanip>
#include <vector>
#include "raylib-cpp.hpp"

using namespace std;

class Grid;

class Sprite {
protected:
	raylib::Vector2 Dir{ 1,0 };
	raylib::Vector2 Origin{ 0,0 };
	raylib::Vector2 Scale{ 1,1 };
	float depth=0; //for sorting
	float animSpeed = 0.8f; //lower value = faster animation
	float Rotation = 0;
	raylib::Rectangle spriteRect={ 0,0,0,0 };
	raylib::Rectangle frameSize = { 0,0,0,0 };
	bool isAnim = false;
	unsigned char totalFrames=0;
	unsigned char frameCount=0;
	float animTime = 0; 
	raylib::Color colour = WHITE;
	string spriteName="";//this is used for finding the texture within the texcache
	string filePath="";
	//raylib::Texture2D texture;
	shared_ptr<raylib::Texture2D> texture;
	raylib::Rectangle dim{ 0,0,0,0 };
	friend class Creature;
public:
	bool isHover = false;
	bool active = true;
	~Sprite() {
		//UnloadTexture(texture);
	}
	enum spriteTYPE
	{
		CREATURE = 0, UI = 1
	};
	spriteTYPE type = spriteTYPE::CREATURE;
	raylib::Vector2 previousGridPos{ 0,0 };
	raylib::Vector2 Position{ 400,400 };
	void Init(raylib::Vector2 position, raylib::Vector2 scale, bool centerOrigin, raylib::Rectangle spriteRect,raylib::Rectangle framerect,int totalframes, float animspeed);
	void Init(raylib::Vector2 position, raylib::Vector2 scale, raylib::Vector2 origin, raylib::Rectangle spriterect);
	virtual void Update(float dTime);
	void PlayAnimation(float dTime);
	virtual void Render();
	Sprite(string spriteName, string path);
	Sprite();
	void setTex(string& filePath, const raylib::Rectangle& texRect);
	void setPos(raylib::Vector2& pos) {
		Position = pos;
	}
	bool setGridPosition(Grid& grid,int x, int y, bool checkCol=true);
	//change grid pos without resetting previous pos to empty
	bool setGridPositionNoPrev(Grid& grid, int x, int y, bool checkCol=true); 
	void setSpriteRect(raylib::Rectangle& SpriteRect) {
		spriteRect = SpriteRect;
	}
	raylib::Rectangle getDim() {
		dim = (raylib::Rectangle{ (spriteRect.x), (spriteRect.y), (spriteRect.width * Scale.x), (spriteRect.height * Scale.y) });
		return dim;
	}
	const raylib::Rectangle getSpriteRect() {
		return spriteRect;
	}
	const raylib::Rectangle getFrameSize() {
		return frameSize;
	}
	void setColour(raylib::Color newCol) {
		colour = newCol;
	}
	void setScale(raylib::Vector2 newScale) {
		Scale = newScale;
	}
	void setAnimValues(int totalframes, float animspeed) {
		totalFrames = totalframes;
		animSpeed = animspeed;
		//reset animation counter
		animTime = 0;
		frameCount = 0;
	}
	void setAnimValues(int totalframes, float animspeed, raylib::Rectangle frameRect) {
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
	UISprite(string spriteName, string path);
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
