#pragma once
#include "WindowUtils.h"
#include "SimpleMath.h"
#include "SpriteFont.h"
#include "CommonStates.h"
#include <stdexcept>
#include "Input.h"
#include "Singleton.h"
#include "creature.h"
#include "ModeMgr.h"
#include "AudioMgrFMOD.h"
#include "Mesh.h"
#include "Model.h"

struct Tile {
	enum Container {NONE=0,CREATURE=1,ABILITY=2};
	Container cellValue = Container::NONE;
};

class Grid {
	Sprite gridSprite;
public: 
	int gridOriginX = 128;
	int gridOriginY = 128;
	int XOFFSET = 15;
	int YOFFSET = 20;
	static constexpr int gridWidth = 3;
	static constexpr int gridHeight = 4;
	static constexpr int cellSize = 128;
	Tile grid[gridWidth][gridHeight];
	bool visible = true;
	Grid(MyD3D& d3d);
	Grid();
    void RenderGrid(float dTime, SpriteBatch* mySpriteBatch);
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

	Mouse mouse;

	Game(MyD3D& md3d);
	void CountTroops();
	void Release();
	void Update(float dTime);
	void Render(float dTime);
	void ProcessKey(char key) {
		mModeMgr.ProcessKey(key);
	}
	void ApplyLua();
	void CreateEnemyGroup();
	int CalculateDifficulty(vector<Creature> creatureGroup);
	//getters
	MyD3D& GetD3D() { return md3d; }
	ModeMgr& GetModeMgr() { return mModeMgr; }
	AudioMgrFMOD& getAudioMgr() { return audioManager; }
	unsigned int musicHdl = 1;
	unsigned int sfxHdl = 2;
	//number of saved troops
	int maxDifficulty = 5;
	vector<int> troopCounts;
private:
	MyD3D& md3d;
	SpriteBatch* mySpriteBatch = nullptr;
	ModeMgr mModeMgr;
	AudioMgrFMOD audioManager;
};
Vector2 getGridPosition(Grid& grid, Vector2 Position);
Vector2 getGridPosition(int gridWidth, int gridHeight, int gridCellSize, int gridOriginX, int gridOriginY, Vector2 Position);

//GAME MODES

class PlayMode : public AMode {
	enum class State{BUILD,SHOP, FIGHT, WIN, LOSE};
	SpriteFont* pixelFont = nullptr;
	SpriteFont* pixelFontSmall = nullptr;
	State state = State::BUILD;

	std::vector<Creature> shopCreatures{};
	std::vector<UISprite> uiSprites{};
	Sprite bgSprite{};
	Sprite coinSprite{};
	Sprite shopSprite{};
	Sprite logoSprite{};
public:
	bool resetShop = true;
	std::vector<Creature> gameCreatures{};
	~PlayMode() 
	{
		delete pixelFont;
		delete pixelFontSmall;
		gameCreatures.clear();
		uiSprites.clear();
	}
	int findClosest(int idx,bool Enemy);
	PlayMode();
	void InitShop();
	void InitBuild();
	static const GAMEMODE MODE_NAME = GAMEMODE::PLAY;
	void Update(float dTime) override;
	void Render(float dTime, SpriteBatch& batch) override;
	void StoreRender(float dtime, SpriteBatch& batch);
	void UIAction(UISprite& sprite);
	void ResetBoard();
	void BuildRender(float dTime, SpriteBatch& batch);
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
	void FightRender(float dTime,SpriteBatch& batch);
	void dragSprite(Sprite& sprite, Mouse& mouse);
	bool isGridClicked(Grid& Grid, Sprite& sprite, Mouse& mouse, bool noPrev=false);
	void spawnEnemy(creatureType enemyToSpawn, Vector2 position);
	void RenderShopTile(Creature& creature, Vector2 tilePosition, SpriteBatch& batch);
	void GenerateScriptEnemies(int difficulty);
	SpriteFont& GetFont() {
		assert(pixelFont);
		return *pixelFont;
	}
	SpriteFont& GetFontSmall() {
		assert(pixelFontSmall);
		return *pixelFontSmall;
	}

	int coins = 20;
private:
	void SetShopPositions();
	void SpawnShopCreatures();
	void GenerateEnemies();
	void GenerateRandomEnemy();
	//call when placing creature
	void PlaceCreatureSFX(Creature& creature);

	const Vector2 baseTilePos = { 561, 133 };
	bool wasClickReleased = false;
	bool spriteDragging = false;
	bool draggingShop = false;
	const unsigned char shopCreatureOffset = 45;
	const unsigned char pixelsBetweenTilesX = 250;
	const unsigned char pixelsBetweenTilesY = 245;
	unsigned char enemiesAlive = 0;
	unsigned char teamAlive = 0;
	unsigned char maxShopSlots = 4;
	unsigned char currentRound = 1;
	int movedSprite = 0; //index of creature being moved
	vector<vector<int>> playedTroops;
};

bool isSpriteClicked(Sprite& sprite, Mouse& mouse);
bool isSpriteClickReleased(Sprite& sprite, Mouse& mouse);