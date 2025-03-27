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

//if ALT+ENTER or resize or drag window we might want do
//something like pause the game perhaps, but we definitely
//need to let D3D know what's happened (OnResize_Default).
int OnResize(lua_State* L)
{
	int screenWidth = lua_tointeger(L, 1);
	int screenHeight = lua_tointeger(L, 2);
	Game::Get().GetD3D().OnResize_Default(screenWidth, screenHeight);
	WinUtil::Get().ResizeWindow(screenWidth, screenHeight);
	//d3d.OnResize_Default(screenWidth, screenHeight);
	lua_pop(L, 2);
	return 1;
}

void removeLastCharFromFile(string fileToChange) {
	std::ifstream fileIn(fileToChange);                   // Open for reading

	std::stringstream buffer;                             // Store contents in a std::string
	buffer << fileIn.rdbuf();
	std::string contents = buffer.str();

	fileIn.close();
	contents.pop_back();                                  // Remove last character


	std::ofstream fileOut(fileToChange, std::ios::trunc); // Open for writing (while also clearing file)
	fileOut << contents;                                  // Output contents with removed character
	fileOut.close();
}

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
	delete mySpriteBatch;
	mySpriteBatch = nullptr;
	troopCounts.clear();
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
	lua_register(L, "OnResize", OnResize);

	CallVoidVoidCFunc(L, "CallResize");
	if (mModeMgr.GetModeName() == GAMEMODE::PLAY) {
		PlayMode* playMode = dynamic_cast<PlayMode*>(mModeMgr.GetMode());
		playMode->coins = (LuaGetInt(L, "coins"));
		Vector2L pos;
		playMode->resetShop = true;
		playMode->InitShop();
	}


	lua_close(L);
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
				Vector2 creatureGridPos = getGridPosition(playMode->grid, creaturesToWrite[i].sprite.Position);
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
			removeLastCharFromFile(("Difficulty" + to_string(difficulty) + ".lua"));
			//need to open Script.lua as it contains the WriteTroops function
			Execute(L, "Script.lua");
			CallWriteTroops(L,difficulty, toWrite.str().c_str());
			//now update the count of troops
			Execute(L, "Difficulty" + to_string(difficulty) + ".lua");
			troopCounts[difficulty-1]=GetTableLength(L, "Troops");
			lua_close(L);

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
	difficulty /= 2;
	if (difficulty < 1)
		difficulty = 1;
	else if (difficulty > 5)
		difficulty = 5;
	return difficulty;
}

