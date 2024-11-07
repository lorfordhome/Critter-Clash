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


class Sprite {
	Vector2 Position{ 400,400 };
	Vector2 Dir{ 1,0 };
	Vector2 Origin{ 0,0 };
	Vector2 Scale{ 3,3 };
	float depth; //for sorting
	float animSpeed = 0.8f; //lower value = faster animation
	float Rotation = 0;
	RECT spriteRect;
	bool isAnim = false;
	RECT frameSize;
	int totalFrames;
	int frameCount=0;
	float animTime = 0; 
	XMVECTOR colour = Colours::White;
	string spriteName;//this is used for finding the texture within the texcache
	string filePath;
	ID3D11ShaderResourceView* texture;;

public:
	void Init(Vector2 position,Vector2 scale,Vector2 origin);
	void Init(Vector2 position, Vector2 scale, Vector2 origin, RECT spriteRect,RECT framerect,int totalframes, float animspeed);
	void Update(float dTime, MyD3D& d3d);
	void Render(MyD3D& d3d, SpriteBatch* Batch);
	Sprite(string spriteName, string path, MyD3D& d3d);
};
