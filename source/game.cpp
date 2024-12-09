#pragma once
#include "game.h"

Vector2 getGridPosition(Grid& grid, Vector2 Position) {
	SimpleMath::Rectangle gRect = SimpleMath::Rectangle({ 0,0,grid.cellSize,grid.cellSize });
	for (int i = 0; i < grid.gridWidth; i++) {
		for (int j = 0; j < grid.gridHeight; j++) {
			gRect.x = (i * grid.cellSize) + grid.gridOriginX;
			gRect.y = (j * grid.cellSize)+grid.gridOriginY;
			if (gRect.Contains(Position)) {
				return Vector2(i, j);
			}
		}
	}
	return Vector2(420, 420);//can't find gridpos
}

Vector2 getGridPosition(int gridWidth,int gridHeight,int gridCellSize,int gridOriginX,int gridOriginY, Vector2 Position) {
	SimpleMath::Rectangle gRect = SimpleMath::Rectangle({ 0,0,gridCellSize,gridCellSize });
	for (int i = 0; i < gridWidth; i++) {
		for (int j = 0; j < gridHeight; j++) {
			gRect.x = (i * gridCellSize) + gridOriginX;
			gRect.y = (j * gridCellSize) + gridOriginY;
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

	for (int i = 0; i < gridWidth; i++) {
		for (int j = 0; j < gridHeight; j++) {
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
		for (int i = 0; i < gridWidth; i++) {
			for (int j = 0; j < gridHeight; j++) {
				Vector2 pos((cellSize * i)+gridOriginX, (cellSize * j)+gridOriginY);
				gridSprite.setPos(pos);
				gridSprite.Render(mySpriteBatch);
			}
		}
	}
}

void Grid::updateTile(int x, int y, Tile::Container Value) {
	grid[x][y].cellValue = Value;
}

void Grid::ResetTiles() {
	for (int i = 0; i < gridWidth; i++) {
		for (int j = 0; j < gridHeight; j++) {
			updateTile(i, j, Tile::Container::NONE);
		}
	}
}

Game::Game(MyD3D& d3d) :md3d(d3d)
{
	File::initialiseSystem();
	audioManager.Initialise();
	mouse.Initialise(WinUtil::Get().GetMainWnd(), true, false);
	mySpriteBatch = new SpriteBatch(&md3d.GetDeviceCtx());
	md3d.GetCache().SetAssetPath("data/");
	assert(mySpriteBatch);
	mModeMgr.AddMode(new PlayMode());
	mModeMgr.AddMode(new MenuMode());
	mModeMgr.SwitchMode(MenuMode::MODE_NAME);
	audioManager.GetSongMgr()->Play(utf8string("MenuMusic"), true, false, &musicHdl, audioManager.GetSongMgr()->GetVolume());
}


void Game::Release() {
	delete mySpriteBatch;
	mySpriteBatch = nullptr;
	mModeMgr.Release();
}

void Game::Update(float dTime)
{
	audioManager.Update();
	mModeMgr.Update(dTime);
}
void Game::Render(float dTime)
{
	md3d.BeginRender(Vector4(0.1843137254901961, 0.5058823529411764, 0.21176470588235294, 1));


	CommonStates dxstate(&md3d.GetDevice());
	mySpriteBatch->Begin(SpriteSortMode_Deferred, dxstate.NonPremultiplied(), dxstate.PointWrap());
	mModeMgr.Render(dTime, *mySpriteBatch);
	mySpriteBatch->End();


	md3d.EndRender();
	mouse.PostProcess();
}

