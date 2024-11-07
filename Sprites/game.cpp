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
#include "game.h"

Game::Game(MyD3D& d3d) :md3d(d3d), mySpriteBatch(nullptr) {
	mySpriteBatch = new SpriteBatch(&md3d.GetDeviceCtx());

	md3d.GetCache().SetAssetPath("data/");
	mySpriteBatch = new SpriteBatch(&d3d.GetDeviceCtx());
	assert(mySpriteBatch);
	Sprite breloomAnim("Breloom", "sleepanim.dds", md3d);
	breloomAnim.Init(Vector2(400, 400), Vector2(3, 3), Vector2(0, 0), RECT{ 0,0,32,32 }, RECT{ 0,0,32,32 }, 2, 0.8f);
	gameSprites.push_back(breloomAnim);

}

void Game::Release() {
	delete mySpriteBatch;
	mySpriteBatch = nullptr;
	gameSprites.erase(gameSprites.begin());
}

void Game::Update(float dTime) {
	for (int i = 0; i < gameSprites.size(); i++) {
		gameSprites[i].Update(dTime, md3d);
	}
}
void Game::Render(float dTime) {
	md3d.BeginRender(Vector4(Colors::CadetBlue));
	CommonStates dxstate(&md3d.GetDevice());
	mySpriteBatch->Begin(SpriteSortMode_Deferred, dxstate.NonPremultiplied());

	for (int i = 0; i < gameSprites.size(); i++) {
		gameSprites[i].Render(md3d,mySpriteBatch);
	}

	mySpriteBatch->End();
	md3d.EndRender();
}