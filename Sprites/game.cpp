#pragma once
#include <windows.h>
#include <string>
#include <cassert>
#include <d3d11.h>
#include <iomanip>
#include <vector>
#include "creature.h"
#include "WindowUtils.h"
#include "D3DUtil.h"
#include "D3D.h"
#include "SimpleMath.h"
#include "SpriteFont.h"
#include "DDSTextureLoader.h"
#include "CommonStates.h"
#include "sprite.h"
#include "game.h"

Vector2 getGridPosition(Grid& grid, Vector2 Position) {
	SimpleMath::Rectangle gRect = SimpleMath::Rectangle({ 0,0,grid.cellSize,grid.cellSize });
	for (int i = 0; i < grid.width; i++) {
		gRect.x = i * grid.cellSize;
		for (int j = 0; j < grid.height; j++) {
			gRect.y = j * grid.cellSize;
			if (gRect.Contains(Position)) {
				return Vector2(i, j);
			}
		}
	}
	return Vector2(420, 420);//can't find gridpos
}


Grid::Grid(MyD3D& d3d) {
	Sprite GridSprite("gridLines", "gridSquare2.dds", d3d); //temporary to initialise grid sprite
	gridSprite = GridSprite;
	gridSprite.setScale(Vector2(4, 4));
	gridSprite.setSpriteRect(RECT{ 0,0,33,33 });

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			grid[i][j].cellValue = Tile::Container::NONE;
		}
	}
}


Grid::Grid() {

}

Tile Grid::Get(int x, int y) {
	return grid[x][y];
}

void Grid::RenderGrid(float dTime,SpriteBatch* mySpriteBatch) {
	if (visible) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				Vector2 pos((cellSize * i), (cellSize * j));
				gridSprite.setPos(pos);
				if (grid[i][j].cellValue == Tile::Container::CREATURE) {
					gridSprite.setColour(Colours::Red);
				}
				else {
					gridSprite.setColour(Colours::White);
				}
				gridSprite.Render(mySpriteBatch);
			}
		}
	}
}

void Grid::updateTile(int x, int y, Tile::Container Value) {
	grid[x][y].cellValue = Value;
}

Game::Game(MyD3D& d3d) :md3d(d3d)
{
	mouse.Initialise(WinUtil::Get().GetMainWnd(), true, false);
	mySpriteBatch = new SpriteBatch(&md3d.GetDeviceCtx());
	md3d.GetCache().SetAssetPath("data/");
	assert(mySpriteBatch);
	mModeMgr.AddMode(new PlayMode());
	mModeMgr.SwitchMode(PlayMode::MODE_NAME);
}


void Game::Release() {
	delete mySpriteBatch;
	mySpriteBatch = nullptr;
	mModeMgr.Release();
}

void Game::Update(float dTime)
{
	mModeMgr.Update(dTime);
}
void Game::Render(float dTime)
{
	md3d.BeginRender(Colours::Black);


	CommonStates dxstate(&md3d.GetDevice());
	mySpriteBatch->Begin(SpriteSortMode_Deferred, dxstate.NonPremultiplied(), dxstate.PointWrap());
	mModeMgr.Render(dTime, *mySpriteBatch);
	mySpriteBatch->End();


	md3d.EndRender();
	mouse.PostProcess();
}

