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

struct Tile {
	enum Container {NONE=0,CREATURE=1,ABILITY=2};
	Container cellValue = Container::NONE;
};

class Grid {
	Sprite gridSprite;
public: 
	static constexpr int width = 20;
	static constexpr int height = 20;
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
	enum class State{START,PLAY,END};
	State state = State::START;

	std::vector<Sprite> gameSprites;
public:
	~Game() {
		Release();
	}

	Mouse mouse;
	Grid grid;
	Game(MyD3D& md3d);
	void Release();
	void Update(float dTime);
	void Render(float dTime);
	void dragSprite(Sprite& sprite);
	bool isSpriteClicked(Sprite& sprite);
	bool isGridClicked(Grid& Grid, Sprite& sprite);
private:
	MyD3D& md3d;
	SpriteBatch* mySpriteBatch = nullptr;
	bool spriteDragging = false;
	int movedSprite;
};