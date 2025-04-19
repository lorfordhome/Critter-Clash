#pragma once
#include "sprite.h"
#include "game.h"
using namespace std;

bool Sprite::setGridPosition(Grid& grid,int x,int y, bool checkCol) {
	if (!checkCol) {
		Position = (raylib::Vector2(float((x * grid.cellSize) + grid.XOFFSET + grid.gridOriginX), float((y * grid.cellSize) + grid.YOFFSET + grid.gridOriginY)));
		return true;
	}
	if (grid.Get(x, y).cellValue != Tile::CREATURE) {
		Position = (raylib::Vector2(float((x * grid.cellSize) + grid.XOFFSET + grid.gridOriginX), float((y * grid.cellSize) + grid.YOFFSET+grid.gridOriginY)));
		grid.updateTile(previousGridPos.x, previousGridPos.y, Tile::NONE);
		grid.updateTile(x, y, Tile::CREATURE);
		return true;
	}
	else 
		return false;
}

bool Sprite::setGridPositionNoPrev(Grid& grid, int x, int y, bool checkCol) {
	if (!checkCol) {
		Position = (raylib::Vector2(float((x * grid.cellSize) + grid.XOFFSET + grid.gridOriginX), float((y * grid.cellSize) + grid.YOFFSET + grid.gridOriginY)));
		return true;
	}
	if (grid.Get(x, y).cellValue != Tile::CREATURE) {
		Position = (raylib::Vector2(float((x * grid.cellSize) + grid.XOFFSET + grid.gridOriginX), float((y * grid.cellSize) + grid.YOFFSET + grid.gridOriginY)));
		grid.updateTile(x, y, Tile::CREATURE);
		return true;
	}
	else
		return false;
}

Sprite::Sprite() {
	texture = nullptr;
};
Sprite::Sprite(string SpriteName, string path) :spriteName(SpriteName),filePath(path) {
	/*texture = d3d.GetCache().LoadTexture(&d3d.GetDevice(), filePath, spriteName, true);*/
}
UISprite::UISprite() {
	texture = nullptr;
	type = Sprite::spriteTYPE::UI;
}
UISprite::UISprite(string SpriteName, string path) {
	spriteName = SpriteName;
	filePath = path;
	//texture = d3d.GetCache().LoadTexture(&d3d.GetDevice(), filePath, spriteName, true);
	type = Sprite::spriteTYPE::UI;
}
void Sprite::Init(Vector2 position, Vector2 scale, Vector2 origin, raylib::Rectangle spriterect)
{
	Position = position;
	Scale = scale;
	Origin = origin;
	spriteRect = spriterect;

}
void Sprite::Init(Vector2 position, Vector2 scale, bool centerOrigin, raylib::Rectangle spriterect, raylib::Rectangle framerect, int totalframes, float animspeed) {
	Scale = scale;
	spriteRect = spriterect;
	frameSize = framerect;
	totalFrames = totalframes;
	animSpeed = animspeed;
	isAnim = true;
	if (centerOrigin) {
	}
}
void Sprite::Update(float dTime)
{
	if (isAnim)
	{
		PlayAnimation(dTime);
	}
}

void Sprite::PlayAnimation(float dTime) {
	animTime += dTime;
	if (animTime > animSpeed)
	{
		if (frameCount < totalFrames - 1) {
			spriteRect.x += frameSize.x;
			++frameCount;
		}
		else
		{
			spriteRect.x = frameSize.x;
			frameCount = 0;
		}
		animTime = 0;
	}
}

void UISprite::Update(float dTime) 
{
	if (isHover)
		colour = raylib::Color::DarkGray();
	else
		colour = raylib::Color::White();
	if (isAnim)
	{
		PlayAnimation(dTime);
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
		int soundIdx = std::rand() % 3;
		if (soundIdx == 0) {
			Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("Click1"), false, false, &Game::Get().sfxHdl);
		}
		if (soundIdx == 1) {
			Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("Click2"), false, false, &Game::Get().sfxHdl);
		}
		if (soundIdx == 2) {
			Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("Click3"), false, false, &Game::Get().sfxHdl);
		}
		return true;
	}
	else {
		return false;
	}
}


void Sprite::Render()
{
	if (active) {
		texture->Draw(spriteRect, raylib::Rectangle(Position.x, Position.y, getDim().width, getDim().height), Origin, Rotation, colour);
	}
}

void Sprite::setTex(raylib::Texture2D* tex, const raylib::Rectangle& texRect) {
	texture = tex;
	spriteRect = texRect;
}

void Sprite::setTex(raylib::Texture2D* tex)
{
	texture = tex;
}
