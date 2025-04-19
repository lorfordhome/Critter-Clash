#pragma once
#include <stdexcept>
#include "Singleton.h"
#include "creature.h"
#include "ModeMgr.h"
#include "AudioMgrFMOD.h"
#include "LuaHelper.h"
#include "raylib-cpp.hpp"
#include "rango.h"

struct Tile {
	enum Container {NONE=0,CREATURE=1,ABILITY=2};
	Container cellValue = Container::NONE;
};

class Grid {
	Sprite gridSprite;
public: 
	int gridOriginX = 128;
	int gridOriginY = 128;
	int XOFFSET = 0;
	int YOFFSET = 0;
	static constexpr int gridWidth = 3;
	static constexpr int gridHeight = 4;
	static constexpr int cellSize = 128;
	Tile grid[gridWidth][gridHeight];
	bool visible = true;
	Grid();
    void RenderGrid(float dTime);
	Tile Get(int x, int y);
	void updateTile(int x, int y, Tile::Container Value);
	void ResetTiles();

};


class Game : public Singleton<Game> {
public:
	Game();
	~Game() {
		Release();
	}

	Game(Rango& rango);
	void CountTroops();
	void Release();
	void RestartGame();
	void Update(float dTime);
	void Render(float dTime);
	void ProcessKey(char key) {
		mModeMgr.ProcessKey(key);
	}
	void ApplyLua();
	void ApplyLuaCheats(bool defeat);
	void CreateEnemyGroup();
	int CalculateDifficulty(vector<Creature> creatureGroup);
	//getters
	Rango& GetRango() { return rango; }
	ModeMgr& GetModeMgr() { return mModeMgr; }
	AudioMgrFMOD& getAudioMgr() { return audioManager; }
	unsigned int musicHdl = 1;
	unsigned int sfxHdl = 2;
	//number of saved troops
	int maxDifficulty = 5;
	vector<int> troopCounts;
private:
	const float difficultyDivisor = 2;
	Rango& rango;
	ModeMgr mModeMgr;
	AudioMgrFMOD audioManager;
};
Vector2 getGridPosition(Grid& grid, Vector2 Position);
Vector2 getGridPosition(int gridWidth, int gridHeight, int gridCellSize, int gridOriginX, int gridOriginY, Vector2 Position);

//GAME MODES

class PlayMode : public AMode {
	//SpriteFont* pixelFont = nullptr;
	//SpriteFont* pixelFontSmall = nullptr;

	std::vector<Creature> shopCreatures{};
	std::vector<UISprite> uiSprites{};
	Sprite bgSprite{};
	Sprite coinSprite{};
	Sprite shopSprite{};
	Sprite logoSprite{};
public:
	bool hasSavedTeam = false;
	enum class State { BUILD, SHOP, FIGHT, WIN, LOSE };
	State state = State::BUILD;
	bool resetShop = true;
	std::vector<Creature> gameCreatures{};
	~PlayMode() 
	{
		//delete pixelFont;
		//delete pixelFontSmall;
		gameCreatures.clear();
		uiSprites.clear();
		lua_close(L);
		L = NULL;
	}
	int findClosest(int idx,bool Enemy);
	PlayMode();
	void InitShop();
	void InitBuild();
	static const GAMEMODE MODE_NAME = GAMEMODE::PLAY;
	void Update(float dTime) override;
	void Render(float dTime) override;
	void StoreRender(float dtime);
	void UIAction(UISprite& sprite);
	void ResetBoard();
	void BuildRender(float dTime);
	GAMEMODE GetMName() const override {
		return GAMEMODE::PLAY;
	}

	Grid grid;
	void BuildUpdate(float dTime);
	void StoreUpdate(float dtime);
	void FightUpdate(float dTime);
	void OverUpdate(float dTime);
	void InitLose();
	void InitWin();
	void InitBattle();
	void FightRender(float dTime);
	void dragSprite(Sprite& sprite);
	bool isGridClicked(Grid& Grid, Sprite& sprite, bool noPrev=false);
	void spawnEnemy(creatureType enemyToSpawn, Vector2 position);
	void RenderShopTile(Creature& creature, Vector2 tilePosition);
	void GenerateScriptEnemies(int difficulty);
	void InitLuaFunctions(Dispatcher& disp);
	void ApplyLuaCheats(bool defeat);
	//SpriteFont& GetFont() {
	//	assert(pixelFont);
	//	return *pixelFont;
	//}
	//SpriteFont& GetFontSmall() {
	//	assert(pixelFontSmall);
	//	return *pixelFontSmall;
	//}

	int coins = 20;
	unsigned char shopCreatureOffsetX = 20;
	unsigned char shopCreatureOffsetY = 0;
	unsigned char currentRound = 1;
	//for lua stuff
	Dispatcher disp;
	lua_State* L;
private:
	void SetShopPositions();
	void SpawnShopCreatures();
	void GenerateEnemies();
	void GenerateRandomEnemy();
	//call when placing creature
	void PlaceCreatureSFX(Creature& creature);

	float timeToDisplaySaveText = 1.f;
	float textTimer = 0.f;
	bool flagRestart = false;
	const Vector2 baseTilePos = { 561, 133 };
	bool wasClickReleased = false;
	bool spriteDragging = false;
	bool draggingShop = false;
	const unsigned char pixelsBetweenTilesX = 250;
	const unsigned char pixelsBetweenTilesY = 245;
	unsigned char enemiesAlive = 0;
	unsigned char teamAlive = 0;
	unsigned char maxShopSlots = 4;
	int movedSprite = 0; //index of creature being moved
	vector<vector<int>> playedTroops;
};

bool isSpriteClicked(Sprite& sprite);
bool isSpriteClickReleased(Sprite& sprite);