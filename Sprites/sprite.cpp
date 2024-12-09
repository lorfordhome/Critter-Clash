#pragma once
#include <windows.h>
#include <string>
#include <cassert>
#include <d3d11.h>
#include "WindowUtils.h"
#include "D3DUtil.h"
#include "D3D.h"
#include "SimpleMath.h"
#include "SpriteFont.h"
#include "DDSTextureLoader.h"
#include "CommonStates.h"
#include "sprite.h"
#include "game.h"
using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

bool Sprite::setGridPosition(Grid& grid,int x,int y, bool checkCol) {
	if (!checkCol) {
		Position = (Vector2(float((x * grid.cellSize) + grid.XOFFSET + grid.gridOriginX), float((y * grid.cellSize) + grid.YOFFSET + grid.gridOriginY)));
		return true;
	}
	if (grid.Get(x, y).cellValue != Tile::CREATURE) {
		Position = (Vector2(float((x * grid.cellSize) + grid.XOFFSET + grid.gridOriginX), float((y * grid.cellSize) + grid.YOFFSET+grid.gridOriginY)));
		grid.updateTile(previousGridPos.x, previousGridPos.y, Tile::NONE);
		grid.updateTile(x, y, Tile::CREATURE);
		return true;
	}
	else 
		return false;
}

bool Sprite::setGridPositionNoPrev(Grid& grid, int x, int y, bool checkCol) {
	if (!checkCol) {
		Position = (Vector2(float((x * grid.cellSize) + grid.XOFFSET + grid.gridOriginX), float((y * grid.cellSize) + grid.YOFFSET + grid.gridOriginY)));
		return true;
	}
	if (grid.Get(x, y).cellValue != Tile::CREATURE) {
		Position = (Vector2(float((x * grid.cellSize) + grid.XOFFSET + grid.gridOriginX), float((y * grid.cellSize) + grid.YOFFSET + grid.gridOriginY)));
		grid.updateTile(x, y, Tile::CREATURE);
		return true;
	}
	else
		return false;
}

Sprite::Sprite() {
	texture = nullptr;
};
Sprite::Sprite(string SpriteName, string path,MyD3D& d3d) :spriteName(SpriteName),filePath(path) {
	texture = d3d.GetCache().LoadTexture(&d3d.GetDevice(), filePath, spriteName, true);
	dim = (RECT{ long(spriteRect.left * Scale.x), long(spriteRect.top * Scale.y), long(spriteRect.right * Scale.x), long(spriteRect.bottom * Scale.y) });
}
UISprite::UISprite() {
	texture = nullptr;
	type = Sprite::spriteTYPE::UI;
}
UISprite::UISprite(string SpriteName, string path, MyD3D& d3d) {
	spriteName = SpriteName;
	filePath = path;
	texture = d3d.GetCache().LoadTexture(&d3d.GetDevice(), filePath, spriteName, true);
	dim = (RECT{ long(spriteRect.left * Scale.x), long(spriteRect.top * Scale.y), long(spriteRect.right * Scale.x), long(spriteRect.bottom * Scale.y) });
	type = Sprite::spriteTYPE::UI;
}
void Sprite::Init(Vector2 position, Vector2 scale, Vector2 origin, RECT spriterect)
{
	Position = position;
	Scale = scale;
	Origin = origin;
	spriteRect = spriterect;

}
void Sprite::Init(Vector2 position, Vector2 scale, bool centerOrigin, RECT spriterect, RECT framerect, int totalframes, float animspeed) {
	Scale = scale;
	spriteRect = spriterect;
	frameSize = framerect;
	totalFrames = totalframes;
	animSpeed = animspeed;
	isAnim = true;
	dim = (RECT{ long(spriteRect.left * Scale.x), long(spriteRect.top * Scale.y), long(spriteRect.right * Scale.x), long(spriteRect.bottom * Scale.y) });
	if (centerOrigin) {
	}
}
void Sprite::Update(float dTime)
{
	if (isAnim)
	{
		animTime += dTime;
		if (animTime > animSpeed)
		{
			if (frameCount<totalFrames-1) {
				spriteRect.left += frameSize.right;
				spriteRect.right += frameSize.right;
				++frameCount;
			}
			else
			{
				spriteRect.left = frameSize.left;
				spriteRect.right = frameSize.right;
				frameCount = 0;
			}
			animTime = 0;
		}
	}
}

void UISprite::Update(float dTime) 
{
	if (isHover)
		colour = Colors::DarkGray;
	else
		colour = Colours::White;
	if (isAnim)
	{
		animTime += dTime;
		if (animTime > animSpeed)
		{
			if (frameCount < totalFrames - 1) {
				spriteRect.left += frameSize.right;
				spriteRect.right += frameSize.right;
				++frameCount;
			}
			else
			{
				spriteRect.left = frameSize.left;
				spriteRect.right = frameSize.right;
				frameCount = 0;
			}
			animTime = 0;
		}
	}

	if (!canBeClicked)
	{
		clickTimer += dTime;
		if (clickTimer >= clickCooldown) {
			canBeClicked = true;
			clickTimer = 0;
		}
	}
}

//returns true if can be clicked
bool UISprite::Action() 
{
	if (canBeClicked) {
		clickTimer = 0; //makes sure clickTimer is reset
		return true;
	}
	else {
		return false;
	}
}


void Sprite::Render(SpriteBatch* Batch)
{
	if (active)
	Batch->Draw(texture, Position, &spriteRect, colour, Rotation, Origin, Scale);
}

void Sprite::setTex(ID3D11ShaderResourceView* tex, const RECT& texRect) {
	texture = tex;
	spriteRect = texRect;
	dim = (RECT{ long(spriteRect.left * Scale.x), long(spriteRect.top * Scale.y), long(spriteRect.right * Scale.x), long(spriteRect.bottom * Scale.y) });
}

void Sprite::setTex(ID3D11ShaderResourceView* tex) 
{
	texture = tex;
}
