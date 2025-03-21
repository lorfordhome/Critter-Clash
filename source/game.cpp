#pragma once
#include "game.h"
#include "WindowUtils.h"
#include "D3D.h"
#include "GeometryBuilder.h"
#include "LuaHelper.h"
#include <fstream>
#include <ctime>
using namespace DirectX;
using namespace DirectX::SimpleMath;


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
	md3d.GetCache().Release();
	audioManager.Shutdown();
}

void Game::Update(float dTime)
{
	audioManager.Update();
	mModeMgr.Update(dTime);
}
void Game::Render(float dTime)
{
	md3d.BeginRender(Vector4(0.1843f, 0.5058f, 0.2117f, 1)); //hexcode for background colour


	CommonStates dxstate(&md3d.GetDevice());
	mySpriteBatch->Begin(SpriteSortMode_Deferred, dxstate.NonPremultiplied(), dxstate.PointWrap());
	mModeMgr.Render(dTime, *mySpriteBatch);
	mySpriteBatch->End();


	md3d.EndRender();
	mouse.PostProcess();
}

void Game::ApplyLua() {
	//init lua
	lua_State* L = luaL_newstate();
	//open main libraries for scripts
	luaL_openlibs(L);
	//load and parse the lua file 
	if (!LuaOK(L, luaL_dofile(L, "Script.lua")))
		assert(false);

	if (mModeMgr.GetModeName() == GAMEMODE::PLAY) {
		PlayMode* playMode = dynamic_cast<PlayMode*>(mModeMgr.GetMode());
		playMode->coins = (LuaGetInt(L, "coins"));
		Vector2L pos;
		pos.Fromlua(L, "enemyBuizel");
		int type = GetType(L, "enemyBuizel");
		playMode->GenerateScriptEnemies(static_cast<creatureType>(type),{pos.x,pos.y});
	}


	lua_close(L);
}

void Game::CreateEnemyGroup() {
	if (mModeMgr.GetModeName() == GAMEMODE::PLAY) {
		PlayMode* playMode = dynamic_cast<PlayMode*>(mModeMgr.GetMode());
		int creatureCount = 0;
		time_t timestamp;
		time(&timestamp);
		//go through creatures, check if there are any to save
		for (int i = 0; i < playMode->gameCreatures.size(); i++)
		{
			//look at the creatures the player has placed
			if (playMode->gameCreatures[i].isEnemy == false)
			{
				std::ofstream LuaFile;
				LuaFile.open("Script.lua", ios::app);
				Vector2 creatureGridPos = getGridPosition(playMode->grid, playMode->gameCreatures[i].sprite.Position);
				LuaFile << "\n"<<timestamp<<"creature" << creatureCount << " = {x = " << creatureGridPos.x << ",y = " << creatureGridPos.y << ",type = " << static_cast<int>(playMode->gameCreatures[i].type) << "}";
				LuaFile.close();
				creatureCount++;
			}
		}
		//create troop
		if (creatureCount != 0) {

			std::string troopName = "Troop";
			troopName.std::string::append(to_string(timestamp));
			std::ofstream LuaFile;
			LuaFile.open("Script.lua", ios::app);
			LuaFile << "\n"<<troopName<< "={";
			for (int i = 0; i < creatureCount; i++) {
				LuaFile <<timestamp<< "creature" << i;
				if (i + 1 < creatureCount)
					LuaFile << ",";

			}
			LuaFile << "}";
			LuaFile.close();

			//calculate difficulty

			//save difficulty
			lua_State* L = luaL_newstate();
			luaL_openlibs(L);
			if (!LuaOK(L, luaL_dofile(L, "Script.lua")))
				assert(false);
			CallAddGroup(L, "dif1_groups", troopName.c_str());
			//close lua
			lua_close(L);

		}
	}
}

