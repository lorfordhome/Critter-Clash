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

Grid::Grid(MyD3D& d3d) {
	Sprite GridSprite("gridLines", "GridSquare.dds", d3d); //temporary to initialise grid sprite
	gridSprite = GridSprite;
	gridSprite.setSpriteRect(RECT{ 0,0,cellSize,cellSize });

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if(j%2==0^i%2==0)
				grid[i][j].cellValue = Tile::Container::CREATURE;
		}
	}
}
Grid::Grid() {

}

Tile Grid::Get(int x, int y) {
	return grid[x][y];
}

void Grid::RenderGrid(float dTime,MyD3D& md3d,SpriteBatch* mySpriteBatch) {
	
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			Vector2 pos((cellSize * i), (cellSize * j));
			gridSprite.setPos(pos);
			if (grid[i][j].cellValue == Tile::Container::CREATURE) {

			}
			else {

			}
			gridSprite.Render(md3d, mySpriteBatch);
		}
	}

}

void Grid::updateTile(int x, int y, Tile::Container Value) {
	grid[x][y].cellValue = Value;
}

Game::Game(MyD3D& d3d) :md3d(d3d), mySpriteBatch(nullptr) {
	mySpriteBatch = new SpriteBatch(&md3d.GetDeviceCtx());
	md3d.GetCache().SetAssetPath("data/");
	mySpriteBatch = new SpriteBatch(&d3d.GetDeviceCtx());
	assert(mySpriteBatch);
	Sprite breloomAnim("Breloom", "sleepanim.dds", md3d);
	breloomAnim.Init(Vector2(400, 400), Vector2(3, 3), Vector2(0, 0), RECT{ 0,0,32,32 }, RECT{ 0,0,32,32 }, 2, 0.8f);
	gameSprites.push_back(breloomAnim);

	Grid _grid(md3d);
	grid = _grid;
	breloomAnim.setGridPosition(grid, 10, 10);

}

void Game::Release() {
	delete mySpriteBatch;
	mySpriteBatch = nullptr;
	gameSprites.erase(gameSprites.begin());
}



void Game::Update(float dTime) {
	for (int i = 0; i < gameSprites.size(); i++) {
		gameSprites[i].Update(dTime, md3d);
		if (isSpriteClicked(gameSprites[i])) {
			Vector2 gridPos = gameSprites[i].getGridPosition(grid);
			gameSprites[i].setGridPosition(grid, gridPos.x+1, gridPos.y+1);
		}
	}
}
void Game::Render(float dTime) {
	md3d.BeginRender(Vector4(Colors::CadetBlue));
	CommonStates dxstate(&md3d.GetDevice());
	mySpriteBatch->Begin(SpriteSortMode_Deferred, dxstate.NonPremultiplied());

	grid.RenderGrid(dTime, md3d, mySpriteBatch);
	for (int i = 0; i < gameSprites.size(); i++) {
		gameSprites[i].Render(md3d,mySpriteBatch);
	}

	mySpriteBatch->End();
	md3d.EndRender();
}

bool Game::isSpriteClicked(Sprite& sprite) {
	Vector2 mousepos = mouse.GetMousePos(true);
	RECT sRect = sprite.getDim();
	SimpleMath::Rectangle cRect = SimpleMath::Rectangle(sRect);
	cRect.x = sprite.Position.x;
	cRect.y = sprite.Position.y;

	if (cRect.Contains(mousepos)) 
	{
		if (mouse.GetMouseButton(Mouse::ButtonT::LBUTTON)) {
			//pressed
			return true;
		}
		else {
			//is hovering over sprite
			//do something here later
		}
		return false;
	}


}