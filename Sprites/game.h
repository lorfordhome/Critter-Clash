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
#include <stdexcept>
#include "Input.h"
#include "Singleton.h"
#include "creature.h"
#include "ModeMgr.h"

struct Tile {
	enum Container {NONE=0,CREATURE=1,ABILITY=2};
	Container cellValue = Container::NONE;
};

class Grid {
	Sprite gridSprite;
public: 
	int gridOriginX = 128;
	int gridOriginY = 128;
	bool visible = true;
	int XOFFSET = 15;
	int YOFFSET = 20;
	static constexpr int width = 3;
	static constexpr int height = 4;
	Tile grid[width][height];
	static constexpr int cellSize = 128;
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
	void Release();
	void Update(float dTime);
	void Render(float dTime);
	void ProcessKey(char key) {
		mModeMgr.ProcessKey(key);
	}

	//getters
	MyD3D& GetD3D() { return md3d; }
	ModeMgr& GetModeMgr() { return mModeMgr; }
private:
	MyD3D& md3d;
	SpriteBatch* mySpriteBatch = nullptr;
	ModeMgr mModeMgr;
};
Vector2 getGridPosition(Grid& grid, Vector2 Position);


//GAME MODES

class PlayMode : public AMode {
	enum class State{BUILD,SHOP, FIGHT, WIN, LOSE};
	State state = State::BUILD;

	std::vector<Creature> gameCreatures{};
	std::vector<UISprite> uiSprites{};
	Sprite bgSprite{};
	Sprite coinSprite{};
	SpriteFont* pixelFont = nullptr;
	Sprite shopSprite{};
	Sprite logoSprite{};
public:
	~PlayMode() 
	{
		delete pixelFont;
		pixelFont = nullptr;
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
	ModeMgr modeManager;
	void BuildUpdate(float dTime);
	void StoreUpdate(float dtime);
	void FightUpdate(float dTime);
	void OverUpdate(float dTime);
	void InitLose();
	void InitWin();
	void InitBattle();
	void FightRender(float dTime,SpriteBatch& batch);
	void dragSprite(Sprite& sprite, Mouse& mouse);
	bool isGridClicked(Grid& Grid, Sprite& sprite, Mouse& mouse);
	void spawnEnemy(creatureType enemyToSpawn, Vector2 position);
	SpriteFont& GetFont() {
		assert(pixelFont);
		return *pixelFont;
	}
private:
	bool wasClickReleased = false;
	bool spriteDragging = false;
	int movedSprite=0;
	unsigned char enemiesAlive = 0;
	unsigned char teamAlive = 0;
	int coins = 10;
};

bool isSpriteClicked(Sprite& sprite, Mouse& mouse);
bool isSpriteClickReleased(Sprite& sprite, Mouse& mouse);