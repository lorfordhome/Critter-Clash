#pragma once
#include <windows.h>
#include <string>
#include <cassert>
#include <d3d11.h>
#include "WindowUtils.h"
#include "D3DUtil.h"
#include "D3D.h"
#include "SpriteFont.h"
#include "DDSTextureLoader.h"
#include "CommonStates.h"
#include "sprite.h"
#include "game.h"

void BuildMode::Render(float dTime, SpriteBatch& batch) 
{
	bgSprite.Render(&batch);
	grid.RenderGrid(dTime, &batch);
	for (int i = 0; i < gameSprites.size(); i++) {
		gameSprites[i].Render(&batch);
	}
	for (int i = 0; i < uiSprites.size(); i++) {
		uiSprites[i].Render(&batch);
	}
}