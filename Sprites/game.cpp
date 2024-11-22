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

Vector2 moveTowards(Vector2 startPos, Vector2 endPos, float speed)
{
	Vector2 newPos= startPos + (endPos - startPos) * speed;
	return newPos;
}

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
	Sprite GridSprite("gridLines", "GridSquare.dds", d3d); //temporary to initialise grid sprite
	gridSprite = GridSprite;
	gridSprite.setSpriteRect(RECT{ 0,0,cellSize,cellSize });

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

void Grid::RenderGrid(float dTime,MyD3D& md3d,SpriteBatch* mySpriteBatch) {
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
				gridSprite.Render(md3d, mySpriteBatch);
			}
		}
	}
}

void Grid::updateTile(int x, int y, Tile::Container Value) {
	grid[x][y].cellValue = Value;
}

Game::Game(MyD3D& d3d) :md3d(d3d), mySpriteBatch(nullptr) {
	mouse.Initialise(WinUtil::Get().GetMainWnd(), true, false);
	mySpriteBatch = new SpriteBatch(&md3d.GetDeviceCtx());
	md3d.GetCache().SetAssetPath("data/");
	mySpriteBatch = new SpriteBatch(&d3d.GetDeviceCtx());
	assert(mySpriteBatch);

	Grid _grid(md3d);
	grid = _grid;
	Grid _Grid(md3d);
	enemyGrid = _Grid;
	enemyGrid.visible = false;
	enemyGrid.width * 2;

	Creature breloom("Breloom", "breloomIdle.dds", md3d);
	breloom.SpriteInit(gameSprites, grid, Vector2(0, 5), Vector2(3, 3), false, RECT{ 0,96,40,144 }, RECT{ 0,0,40,48 }, 12, 0.1f);
	gameCreatures.push_back(breloom);

	Creature Skitty("Skitty", "skittyIdle.dds", md3d);
	Skitty.SpriteInit(gameSprites,grid, Vector2(2, 5), Vector2(3, 3), true, RECT{ 0,80,32,120 }, RECT{ 0,0,32,40 }, 4, 0.4f);
	gameCreatures.push_back(Skitty);

	spawnEnemy();


	Sprite Button("Button", "startButton.dds", md3d);
	Button.Init(Vector2(430, 50), Vector2(0.15, 0.15), Vector2(0, 0), RECT{ 0,0,1280,427 });
	Button.type = Sprite::spriteTYPE::UI;
	uiSprites.push_back(Button);
}

void Game::Release() {
	delete mySpriteBatch;
	mySpriteBatch = nullptr;
	gameSprites.erase(gameSprites.begin());
}

void Game::BuildUpdate(float dTime)
{
	//UPDATE UI
	for (int i = 0; i < uiSprites.size(); i++)
	{
		uiSprites[i].Update(dTime, md3d);
		if (isSpriteClickReleased(uiSprites[i]) && !spriteDragging)
		{
			InitBattle();
		}
	}

	for (int i = 0; i < gameSprites.size(); i++) {
		gameSprites[i].Update(dTime, md3d);
		if (!spriteDragging) {
			//has player clicked a sprite?
			if (isSpriteClicked(gameSprites[i]) && spriteDragging == false && gameCreatures[i].isEnemy == false) {
				gameSprites[i].previousGridPos = getGridPosition(grid, gameSprites[i].Position);//store grid position in case it needs to be reset
				spriteDragging = true;
				movedSprite = i; //store the sprite that is being moved
			}
		}
	}
	//is player currently trying to move a sprite?
	if (spriteDragging) {
		dragSprite(gameSprites[movedSprite]);
		if (mouse.isClickRelease()) {
			if (isGridClicked(grid, gameSprites[movedSprite]))
			{
				//pressed
				spriteDragging = false;
			}
			else {
				//user tried to place sprite in invalid position - reset the sprite to original pos
				gameSprites[movedSprite].setGridPosition(grid, gameSprites[movedSprite].previousGridPos.x, gameSprites[movedSprite].previousGridPos.y, false);
				spriteDragging = false;
			}
		}

	}
}

void Game::FightUpdate(float dTime) 
{
	for (int i = 0; i < gameSprites.size(); i++) 
	{
		gameSprites[i].Update(dTime, md3d);

		//gameSprites[0].setPos(moveTowards(gameSprites[0].Position, gameSprites[2].Position, 10.f));
	}
}

void Game::InitBattle() 
{
	for (int i = 0; i < uiSprites.size(); i++)
	{
		uiSprites[i].active = false;
	}


	state = State::FIGHT;

}

void Game::Update(float dTime) 
{
	switch (state) {
	case(State::BUILD):
			BuildUpdate(dTime);
			break;
	case(State::FIGHT):
		FightUpdate(dTime);
		break;
	}
}

void Game::Render(float dTime) {
	md3d.BeginRender(Vector4(Colors::CadetBlue));
	CommonStates dxstate(&md3d.GetDevice());
	mySpriteBatch->Begin(SpriteSortMode_Deferred, dxstate.NonPremultiplied(),dxstate.PointWrap());

	grid.RenderGrid(dTime, md3d, mySpriteBatch);
	for (int i = 0; i < gameSprites.size(); i++) {
		gameSprites[i].Render(md3d,mySpriteBatch);
	}
	for (int i = 0; i < uiSprites.size(); i++) {
		uiSprites[i].Render(md3d, mySpriteBatch);
	}

	mySpriteBatch->End();
	md3d.EndRender();
	mouse.PostProcess();
}

void Game::spawnEnemy() 
{
	Creature Buizel("Buizel", "buizelIdle.dds", md3d,true);
	Buizel.SpriteInit(gameSprites, enemyGrid, Vector2(6, 4), Vector2(3, 3), true, RECT{ 0,96,32,144 }, RECT{ 0,0,32,48 }, 9, 0.2f);
	gameCreatures.push_back(Buizel);
}


void Game::dragSprite(Sprite& sprite) {
	sprite.setPos(mouse.GetMousePos(true));
}

bool Game::isSpriteClicked(Sprite& sprite) {
	Vector2 mousepos = mouse.GetMousePos(true);
	RECT sRect = sprite.getDim();
	SimpleMath::Rectangle cRect = SimpleMath::Rectangle(sRect);
	cRect.x = sprite.Position.x;
	cRect.y = sprite.Position.y;

	if (cRect.Contains(mousepos)) 
	{
		if (mouse.GetMouseButton(Mouse::LBUTTON)) {
			//pressed
			return true;
		}
		else {
			//is hovering over sprite
			//do something here later
			if (sprite.type == Sprite::spriteTYPE::UI) {
				sprite.setColour(Colors::DarkGreen);
			}
		}
		return false;
	}


}

bool Game::isSpriteClickReleased(Sprite& sprite) 
{
	Vector2 mousepos = mouse.GetMousePos(true);
	RECT sRect = sprite.getDim();
	SimpleMath::Rectangle cRect = SimpleMath::Rectangle(sRect);
	cRect.x = sprite.Position.x;
	cRect.y = sprite.Position.y;

	if (cRect.Contains(mousepos))
	{
		if (mouse.isClickRelease()) {
			//pressed
			return true;
		}
		else {
			//is hovering over sprite
			//do something here later
			if (sprite.type == Sprite::spriteTYPE::UI) {
				sprite.setColour(Colors::DarkGreen);
			}
		}
		return false;
	}
}

bool Game::isGridClicked(Grid& Grid,Sprite& sprite) {
			Vector2 gridPos = getGridPosition(grid,sprite.Position);

			//has player clicked within the grid?
			if (mouse.isClickRelease() && getGridPosition(grid,mouse.GetMousePos(true)) != Vector2(420, 420))
			{
				if (gridPos == getGridPosition(grid, mouse.GetMousePos(true)))
				{
					if(sprite.setGridPosition(grid, gridPos.x, gridPos.y))
						return true;
				}
			}
	return false;
}

