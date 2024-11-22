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
	bool visible = true;
	int XOFFSET = 15;
	int YOFFSET = 20;
	static constexpr int width = 4;
	static constexpr int height = 6;
	Tile grid[width][height];
	long cellSize = 128;
		Grid(MyD3D& d3d);
		Grid();
      void RenderGrid(float dTime, MyD3D& md3d, SpriteBatch* mySpriteBatch);
      void SetValues();
	  Tile Get(int x, int y);
	  void updateTile(int x, int y, Tile::Container Value);

};



class Game : public Singleton<Game> {
	enum class State{BUILD, FIGHT, END};
	State state = State::BUILD;

	std::vector<Sprite> gameSprites;
	std::vector<Creature> gameCreatures;
	std::vector<Sprite> uiSprites;
public:
	~Game() {
		Release();
	}

	Mouse mouse;
	Grid grid;
	Grid enemyGrid;
	ModeMgr modeManager;
	Game(MyD3D& md3d);
	void BuildUpdate(float dTime);
	void FightUpdate(float dTime);
	void InitBattle();
	void Release();
	void Update(float dTime);
	void Render(float dTime);
	void dragSprite(Sprite& sprite);
	bool isSpriteClicked(Sprite& sprite);
	bool isSpriteClickReleased(Sprite& sprite);
	bool isGridClicked(Grid& Grid, Sprite& sprite);
	void spawnEnemy();
private:
	MyD3D& md3d;
	SpriteBatch* mySpriteBatch = nullptr;
	bool spriteDragging = false;
	int movedSprite;
};
Vector2 getGridPosition(Grid& grid, Vector2 Position);