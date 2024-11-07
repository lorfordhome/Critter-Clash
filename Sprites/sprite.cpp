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
using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

//ID3D11ShaderResourceView* LoadTexture(MyD3D& d3d, wstring path) {
//	ID3D11ShaderResourceView* texture;
//	DDS_ALPHA_MODE alpha;
//	if (CreateDDSTextureFromFile(&d3d.GetDevice(), path.c_str(), nullptr, &texture, 0, &alpha) != S_OK) {
//		WDBOUT(L"Cannot load ");
//		assert(false);
//	}
//	assert(texture);
//	return(texture);
//}
Sprite::Sprite(string SpriteName, string path,MyD3D& d3d) :spriteName(SpriteName),filePath(path) {
	texture = d3d.GetCache().LoadTexture(&d3d.GetDevice(), filePath, spriteName, true);
}
void Sprite::Init(Vector2 position, Vector2 scale, Vector2 origin)
{
	Position = position;
	Scale = scale;
	Origin = origin;

}
void Sprite::Init(Vector2 position, Vector2 scale, Vector2 origin, RECT spriterect, RECT framerect, int totalframes, float animspeed) {
	Position = position;
	Scale = scale;
	Origin = origin;
	spriteRect = spriterect;
	frameSize = framerect;
	totalFrames = totalframes;
	animSpeed = animspeed;
	isAnim = true;
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
