#pragma once
#include "game.h"
#include "LuaHelper.h"
#include <fstream>
#include <ctime>
#include <sstream>


int OnResize(lua_State* L)
{
	//get values from lua
	int screenWidth = lua_tointeger(L, 1);
	int screenHeight = lua_tointeger(L, 2);
	//resize the window
	//WinUtil::Get().ResizeWindow(screenWidth, screenHeight);
	//need to let d3d know whats happened
	//Game::Get().GetD3D().OnResize_Default(screenWidth, screenHeight);
	//clear stack
	lua_pop(L, 2);
	return 1;
}

raylib::Vector2 getGridPosition(Grid& grid, raylib::Vector2 Position) {
	raylib::Rectangle gRect(0, 0, grid.cellSize, grid.cellSize);
	for (int i = 0; i < grid.gridWidth; i++) {
		for (int j = 0; j < grid.gridHeight; j++) {
			gRect.x = (i * grid.cellSize) + grid.gridOriginX;
			gRect.y = (j * grid.cellSize)+grid.gridOriginY;
			if (gRect.CheckCollision(Position)) {
				return raylib::Vector2(i, j);
			}
		}
	}
	return raylib::Vector2(420, 420);//can't find gridpos
}

raylib::Vector2 getGridPosition(int gridWidth,int gridHeight,int gridCellSize,int gridOriginX,int gridOriginY, raylib::Vector2 Position) {
	raylib::Rectangle gRect( 0,0,gridCellSize,gridCellSize );
	for (int i = 0; i < gridWidth; i++) {
		for (int j = 0; j < gridHeight; j++) {
			gRect.x = (i * gridCellSize) + gridOriginX;
			gRect.y = (j * gridCellSize) + gridOriginY;
			if (gRect.CheckCollision(Position)) {
				return raylib::Vector2(i, j);
			}
		}
	}
	return raylib::Vector2(420, 420);//can't find gridpos
}


Grid::Grid() {
	Sprite GridSprite("gridLines", "gridSquare2.dds"); //temporary to initialise grid sprite
	gridSprite = GridSprite;
	gridSprite.setScale(raylib::Vector2(4, 4));
	gridSprite.setSpriteRect(raylib::Rectangle{ 0,0,33,33 });

	for (int i = 0; i < gridWidth; i++) {
		for (int j = 0; j < gridHeight; j++) {
			grid[i][j].cellValue = Tile::Container::NONE;
		}
	}
}


Tile Grid::Get(int x, int y) {
	return grid[x][y];
}

void Grid::RenderGrid(float dTime) {
	if (visible) {
		for (int i = 0; i < gridWidth; i++) {
			for (int j = 0; j < gridHeight; j++) {
				raylib::Vector2 pos((cellSize * i)+gridOriginX, (cellSize * j)+gridOriginY);
				gridSprite.setPos(pos);
				gridSprite.Render();
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

Game::Game(Rango& mrango):rango(mrango)
{
	File::initialiseSystem();
	audioManager.Initialise();
	//mouse.Initialise(WinUtil::Get().GetMainWnd(), true, false);
	//mySpriteBatch = new SpriteBatch(&md3d.GetDeviceCtx());
	rango.GetCache().SetAssetPath("data/");
	assert(mySpriteBatch);
	mModeMgr.AddMode(new PlayMode());
	mModeMgr.AddMode(new MenuMode());
	mModeMgr.SwitchMode(MenuMode::MODE_NAME);
	audioManager.GetSongMgr()->Play(utf8string("MenuMusic"), true, false, &musicHdl, audioManager.GetSongMgr()->GetVolume());

	CountTroops();

}

void Game::CountTroops() {
	//initialise troop count
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	for (int i = 0; i < maxDifficulty; i++) {
		Execute(L, "Difficulty" + to_string(i + 1) + ".lua");
		troopCounts.push_back(GetTableLength(L, "Troops"));
	}
	lua_close(L);
}
void Game::Release() {
	/*delete mySpriteBatch;
	mySpriteBatch = nullptr;*/
	troopCounts.clear();
	mModeMgr.Release();
	rango.GetCache().Release();
	audioManager.Shutdown();
}

void Game::RestartGame() {
	GetModeMgr().DeleteMode(GAMEMODE::PLAY);
	GetModeMgr().AddMode(new PlayMode());
	GetModeMgr().SwitchMode(GAMEMODE::PLAY);
	//reset music
	if (getAudioMgr().GetSongMgr()->IsPlaying(Game::Get().musicHdl)) {
		getAudioMgr().GetSongMgr()->Stop();
		getAudioMgr().GetSongMgr()->Play(utf8string("MenuMusic"), true, false, &Game::Get().musicHdl, Game::Get().getAudioMgr().GetSongMgr()->GetVolume());
	}
}

void Game::Update(float dTime)
{
	audioManager.Update();
	mModeMgr.Update(dTime);
}
void Game::Render(float dTime)
{
	BeginDrawing();
	ClearBackground(GREEN);

	mModeMgr.Render(dTime);

	EndDrawing();
}

void Game::ApplyLua() {
	//init lua
	lua_State* L = luaL_newstate();
	//open main libraries for scripts
	luaL_openlibs(L);

	
	//load and parse the lua file 
	if (!LuaOK(L, luaL_dofile(L, "Script.lua")))
		assert(false);
	//set up and call OnResize function
	lua_register(L, "OnResize", OnResize);
	CallVoidVoidCFunc(L, "CallResize");

	//apply values from lua
	if (mModeMgr.GetModeName() == GAMEMODE::PLAY) {
		PlayMode* playMode = dynamic_cast<PlayMode*>(mModeMgr.GetMode());
		playMode->shopCreatureOffsetX = (LuaGetInt(L, "shopCreatureOffsetX"));
		playMode->shopCreatureOffsetY = (LuaGetInt(L, "shopCreatureOffsetY"));
		playMode->coins = (LuaGetInt(L, "coins"));
		playMode->currentRound = (LuaGetInt(L, "currentRound"));
		if (playMode->state == PlayMode::State::SHOP) {
			playMode->resetShop = true;
			playMode->InitShop();
		}
	}


	lua_close(L);
}

void Game::ApplyLuaCheats(bool defeat) {
	if (mModeMgr.GetModeName() == GAMEMODE::PLAY) {
		PlayMode* playMode = dynamic_cast<PlayMode*>(mModeMgr.GetMode());
		if (playMode->state == PlayMode::State::FIGHT) {
			playMode->ApplyLuaCheats(defeat);
		}
	}
}

void Game::CreateEnemyGroup() {
	if (mModeMgr.GetModeName() == GAMEMODE::PLAY) {
		PlayMode* playMode = dynamic_cast<PlayMode*>(mModeMgr.GetMode());
		int creatureCount = 0;

		//go through creatures, check if there are any to save
		vector<Creature> creaturesToWrite;
		for (int i = 0; i < playMode->gameCreatures.size(); i++)
		{
			//look at the creatures the player has placed
			if (playMode->gameCreatures[i].isEnemy == false)
			{
				creaturesToWrite.push_back(playMode->gameCreatures[i]);
			}
		}
		//create troop
		if (creaturesToWrite.size() != 0) {
			int difficulty = CalculateDifficulty(creaturesToWrite);
			stringstream toWrite;
			if (troopCounts[difficulty-1] != 0)
				toWrite << ",\n";
			toWrite << "{";


			for (int i = 0; i < creaturesToWrite.size(); i++) {
				raylib::Vector2 creatureGridPos = getGridPosition(playMode->grid, creaturesToWrite[i].sprite.Position);
				toWrite <<"{x = " << creatureGridPos.x << ",y = " << creatureGridPos.y << ",type = " << static_cast<int>(playMode->gameCreatures[i].type) << "}";
				if (i + 1 < creaturesToWrite.size())
					toWrite << ",";

			}
			toWrite << "}}";
			//write to file
			lua_State* L = luaL_newstate();
			luaL_openlibs(L);
			Execute(L, "Difficulty" + to_string(difficulty) + ".lua");
			//need to remove last char from file so the Troops table will be closed correctly
			RemoveLastCharFromFile(("Difficulty" + to_string(difficulty) + ".lua"));
			//need to open Script.lua as it contains the WriteTroops function
			Execute(L, "Script.lua");
			CallWriteTroops(L,difficulty, toWrite.str().c_str());
			//now update the count of troops
			Execute(L, "Difficulty" + to_string(difficulty) + ".lua");
			troopCounts[difficulty-1]=GetTableLength(L, "Troops");
			lua_close(L);

			playMode->hasSavedTeam = true;
		}
	}
}

int Game::CalculateDifficulty(vector<Creature> creatureGroup) {
	if (creatureGroup.size() < 1)
		return 0;
	float difficulty = 0;
	for (int i = 0; i < creatureGroup.size(); i++) {
		difficulty+=creatureGroup[i].upgradeLevel;
	}
	difficulty /= difficultyDivisor;
	if (difficulty < 1)
		difficulty = 1;
	else if (difficulty > maxDifficulty)
		difficulty = maxDifficulty;
	return difficulty;
}

