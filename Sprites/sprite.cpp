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

void Sprite::setGridPosition(Grid& grid,int x,int y) {
	Position=(Vector2(x * grid.cellSize, y * grid.cellSize));
}

Vector2 Sprite::getGridPosition(Grid& grid) {
	SimpleMath::Rectangle gRect = SimpleMath::Rectangle({ 0,0,grid.cellSize,grid.cellSize });
	for (int i = 0; i < grid.width; i++) {
		gRect.x = i*grid.cellSize;
		for (int j = 0; j < grid.height; j++) {
			gRect.y = j*grid.cellSize;
			if (gRect.Contains(Position)) {
				return Vector2(i, j);
			}
		}
	}
	return Vector2(0,0);//can't find gridpos
}

Sprite::Sprite() {
	texture = nullptr;
};
Sprite::Sprite(string SpriteName, string path,MyD3D& d3d) :spriteName(SpriteName),filePath(path) {
	texture = d3d.GetCache().LoadTexture(&d3d.GetDevice(), filePath, spriteName, true);
	dim = (RECT{ long(spriteRect.left * Scale.x), long(spriteRect.top * Scale.y), long(spriteRect.right * Scale.x), long(spriteRect.bottom * Scale.y) });
}
void Sprite::Init(Vector2 position, Vector2 scale, Vector2 origin)
{
	Position = position;
	Scale = scale;
	Origin = origin;

}
void Sprite::Init(Grid& grid,Vector2 position, Vector2 scale, bool centerOrigin, RECT spriterect, RECT framerect, int totalframes, float animspeed) {
	setGridPosition(grid, position.x,position.y);
	Scale = scale;
	spriteRect = spriterect;
	frameSize = framerect;
	totalFrames = totalframes;
	animSpeed = animspeed;
	isAnim = true;
	dim = (RECT{ long(spriteRect.left * Scale.x), long(spriteRect.top * Scale.y), long(spriteRect.right * Scale.x), long(spriteRect.bottom * Scale.y) });
	if (centerOrigin) {
		Origin = Vector2(spriteRect.right / 2, spriteRect.bottom / 2);
	}
}
void Sprite::Update(float dTime, MyD3D& d3d)
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
				spriteRect.left = 0;
				spriteRect.right = frameSize.right;
				frameCount = 0;
			}
			animTime = 0;
		}
	}
}
void Sprite::Render(MyD3D& d3d, SpriteBatch* Batch)
{
	Batch->Draw(texture, Position, &spriteRect, colour, Rotation, Origin, Scale);
}

void Sprite::setTex(ID3D11ShaderResourceView& tex, const RECT& texRect) {
	texture = &tex;
	spriteRect = texRect;
	dim = (RECT{ long(spriteRect.left * Scale.x), long(spriteRect.top * Scale.y), long(spriteRect.right * Scale.x), long(spriteRect.bottom * Scale.y) });
}
