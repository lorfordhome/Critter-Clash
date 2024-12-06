#pragma once
#include <windows.h>
#include <string>
#include <cassert>
#include <d3d11.h>
#include "WindowUtils.h"
#include "D3DUtil.h"
#include "D3D.h"
#include "SpriteFont.h"
#include "DDSTextureLoader.h"
#include "CommonStates.h"
#include "sprite.h"
#include "game.h"
#include <SpriteFont.h> //for text rendering


Vector2 MoveTowards(Vector2 current, Vector2 target, float maxDistanceDelta)
{
	Vector2 a = target - current;
	a.Normalize();
	float magnitude = (a.x * a.x + a.y * a.y);
	if (magnitude <= maxDistanceDelta || magnitude == 0.f)
	{
		return target;
	}
	return current + a * maxDistanceDelta;
	//return current + a / magnitude * maxDistanceDelta;
}

bool checkCol(Sprite& spriteA, Sprite& spriteB) 
{
	//intialise Rectangles
	SimpleMath::Rectangle aRect = SimpleMath::Rectangle(spriteA.getDim());
	aRect.x = spriteA.Position.x;
	aRect.y = spriteA.Position.y;
	SimpleMath::Rectangle bRect = SimpleMath::Rectangle(spriteB.getDim());
	bRect.x = spriteB.Position.x;
	bRect.y = spriteB.Position.y;

	return(aRect.Intersects(bRect));

}

bool checkCol(Creature& attacker, Creature& target) 
{
	//create col rectangle for the creatures attack
	SimpleMath::Rectangle aRect(attacker.sprite.Position.x, attacker.sprite.Position.y, attacker.attackRange, attacker.attackRange);
	//create col rectangle for target
	SimpleMath::Rectangle bRect = SimpleMath::Rectangle(target.sprite.getDim());
	bRect.x = target.sprite.Position.x;
	bRect.y = target.sprite.Position.y;

	return(aRect.Intersects(bRect));

}

int PlayMode::findClosest(int idx,bool Enemy) 
{
	Vector2 x = gameCreatures[idx].getSprite().Position;
	float tempPos = 1000000;
	int closestIdx = 0;
	for(int i=0;i<gameCreatures.size();++i)
	{
		if (i != idx) //stops it from checking itself
		{
			if (gameCreatures[i].active&&((!Enemy && gameCreatures[i].isEnemy) || (Enemy && !gameCreatures[i].isEnemy))) //only check opposite team
			{
				Vector2 y = gameCreatures[i].getSprite().Position;
				float Distance = sqrt(pow((x.x - y.x), 2) + pow((x.y - y.y), 2));
				//float magnitude = (distance.x * distance.x + distance.y * distance.y);
				if (Distance < tempPos) //if this is the smallest distance found so far
				{
					tempPos = Distance;
					closestIdx = i;
				}
			}
		}
	}

	return closestIdx;
}


PlayMode::PlayMode() {
	gameCreatures.reserve(10);
	Sprite _bg("Background", "map4.dds", Game::Get().GetD3D());
	_bg.Init(Vector2(0, 0), Vector2(2, 2), Vector2(0, 0), RECT{ 0,0,512,384 });
	bgSprite = _bg;

	Grid _grid(Game::Get().GetD3D()); //temp for initialising
	grid = _grid;

	Creature breloom(creatureType::BRELOOM, Vector2(0, 3), grid);
	gameCreatures.push_back(breloom);

	Creature skitty(creatureType::SKITTY, Vector2(2, 3), grid);
	gameCreatures.push_back(skitty);

	Sprite _coin("coin", "coin.dds", Game::Get().GetD3D());
	_coin.Init(Vector2(5, 20), Vector2(0.8, 0.8), Vector2(0, 0), RECT{ 0,0,45,48 });
	coinSprite = _coin;

	pixelFont = new SpriteFont(&Game::Get().GetD3D().GetDevice(), L"../bin/data/pixelText.spritefont");
	assert(pixelFont);


	InitBuild();


}

void PlayMode::InitShop() 
{
	Sprite _sprite("store", "store.dds", Game::Get().GetD3D());
	_sprite.Init(Vector2(525, 0), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,512,768 });
	shopSprite = _sprite;
	state = State::SHOP;
}

void PlayMode::InitBuild() 
{
	spriteDragging = false;

	//spawn enemies
	spawnEnemy(BUIZEL, Vector2(2, 3));
	spawnEnemy(BUIZEL, Vector2(2, 1));

	UISprite Button("playButton", "playButton.dds", Game::Get().GetD3D());
	Button.Init(Vector2(550, 50), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,144,72 });
	Button.type = Sprite::spriteTYPE::UI;
	Button.uiType = UISprite::UITYPE::start;
	uiSprites.push_back(Button);

	UISprite Button2("storeButton", "storeButton.dds", Game::Get().GetD3D());
	Button2.Init(Vector2(300, 50), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,144,72 });
	Button2.type = Sprite::spriteTYPE::UI;
	Button2.uiType = UISprite::UITYPE::store;
	uiSprites.push_back(Button2);

	state = State::BUILD;
}

void PlayMode::BuildUpdate(float dTime)
{
	//pull mouse now to save on performance
	Mouse& mouse = Game::Get().mouse;
	//UPDATE UI
	for (int i = 0; i < uiSprites.size(); i++)
	{
		uiSprites[i].Update(dTime);
		if (isSpriteClickReleased(uiSprites[i],mouse) && !spriteDragging)
		{
			if (uiSprites[i].Action()) {
				UIAction(uiSprites[i]);
				if (uiSprites.size() > 0) {
					uiSprites[i].canBeClicked = false;
				}
			}
		}
	}

	for (int i = 0; i < gameCreatures.size(); i++) {
		gameCreatures[i].sprite.Update(dTime);
		if (!spriteDragging) {
			//has player clicked a sprite?
			if (isSpriteClicked(gameCreatures[i].sprite, mouse) && !spriteDragging && !gameCreatures[i].isEnemy) {
				gameCreatures[i].sprite.previousGridPos = getGridPosition(grid, gameCreatures[i].sprite.Position);//store grid position in case it needs to be reset
				spriteDragging = true;
				movedSprite = i; //store the sprite that is being moved
			}
		}
	}
	//is player currently trying to move a sprite?
	if (spriteDragging) {
		dragSprite(gameCreatures[movedSprite].sprite, mouse);
		if (mouse.isClickRelease()) {
			if (isGridClicked(grid, gameCreatures[movedSprite].sprite, mouse))
			{
				//pressed
				spriteDragging = false;
			}
			else {
				//user tried to place sprite in invalid position - reset the sprite to original pos
				gameCreatures[movedSprite].sprite.setGridPosition(grid, gameCreatures[movedSprite].sprite.previousGridPos.x, gameCreatures[movedSprite].sprite.previousGridPos.y, false);
				spriteDragging = false;
			}
		}

	}
}

void PlayMode::StoreUpdate(float dtime) 
{
	//we need all the same functions that are used in build update, so might as well just call it...
	BuildUpdate(dtime);



}

void PlayMode::FightUpdate(float dTime)
{
	for (int i = 0; i < gameCreatures.size(); i++) 
	{
		//update returns false if the creature has died
		if (!gameCreatures[i].Update(dTime, true))
		{
			if (gameCreatures[i].isEnemy)
				enemiesAlive--;
			else
				teamAlive--;
		}
		gameCreatures[i].targetIndex = findClosest(i, gameCreatures[i].isEnemy); //find closest potential target

		if(!checkCol(gameCreatures[i], gameCreatures[gameCreatures[i].targetIndex])) //if the creature isn't already within attack range of it's target
		{
			Vector2 posToMove= MoveTowards(gameCreatures[i].sprite.Position,
				gameCreatures[gameCreatures[i].targetIndex].sprite.Position,
				gameCreatures[i].speed);
			posToMove.x* dTime;
			posToMove.y* dTime;
			gameCreatures[i].sprite.setPos(posToMove);
		}
		else
		{
			if (gameCreatures[i].readyToAttack)
				gameCreatures[i].Attack(gameCreatures[gameCreatures[i].targetIndex]);
		}
	}
	//check for end
	if (enemiesAlive <= 0)
		InitWin();
	if (teamAlive <= 0)
		InitLose();
}

void PlayMode::OverUpdate(float dTime) 
{
	for (int i = 0; i < gameCreatures.size(); i++)
	{
		gameCreatures[i].Update(dTime, true);
	}

	Mouse& mouse = Game::Get().mouse;
	//UPDATE UI
	for (int i = 0; i < uiSprites.size(); i++)
	{
		uiSprites[i].Update(dTime);
		if (isSpriteClickReleased(uiSprites[i], mouse) && !spriteDragging)
		{
			UIAction(uiSprites[i]);
		}
	}
}

void PlayMode::InitLose() 
{
	state = State::LOSE;
	//init text
	Sprite Logo("defeatText", "defeatText.dds", Game::Get().GetD3D());
	Logo.Init(Vector2(350, 50), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,313,73 });
	logoSprite = Logo;
	//init ui
	UISprite Button3("homeButton", "homeButton.dds", Game::Get().GetD3D());
	Button3.Init(Vector2(400, 300), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,144,72 });
	Button3.type = Sprite::spriteTYPE::UI;
	Button3.uiType = UISprite::UITYPE::menu;
	uiSprites.push_back(Button3);

	UISprite Button("restartButton", "restartButton.dds", Game::Get().GetD3D());
	Button.Init(Vector2(400, 200), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,144,72 });
	Button.type = Sprite::spriteTYPE::UI;
	Button.uiType = UISprite::UITYPE::restart;
	uiSprites.push_back(Button);
}
void PlayMode::InitWin()
{
	state = State::WIN;
	//init text
	Sprite Logo("victoryText", "victoryText.dds", Game::Get().GetD3D());
	Logo.Init(Vector2(350, 50), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,316,91 });
	logoSprite = Logo;
	//init ui
	UISprite Button("nextButton", "nextButton.dds", Game::Get().GetD3D());
	Button.Init(Vector2(420, 200), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,144,72 });
	Button.type = Sprite::spriteTYPE::UI;
	Button.uiType = UISprite::UITYPE::next;
	uiSprites.push_back(Button);
}

void PlayMode::InitBattle()
{
	//store grid position before fight
	for (int i = 0; i < gameCreatures.size(); ++i) {
		if (!gameCreatures[i].isEnemy)
			gameCreatures[i].idleSprite.previousGridPos = getGridPosition(grid, gameCreatures[i].sprite.Position);
	}
	uiSprites.clear();
	state = State::FIGHT;
}

void PlayMode::Update(float dTime)
{
	switch (state) {
	case(State::BUILD):
		BuildUpdate(dTime);
		break;
	case(State::FIGHT):
		FightUpdate(dTime);
		break;
	case(State::WIN):
		OverUpdate(dTime);
		break;
	case(State::LOSE):
		OverUpdate(dTime);
		break;
	case(State::SHOP):
		StoreUpdate(dTime);
		break;
	}
}

void PlayMode::Render(float dTime, SpriteBatch& batch) {
	switch (state) {
	case(State::BUILD):
		BuildRender(dTime, batch);
		break;
	case(State::FIGHT):
		FightRender(dTime,batch);
		break;
	case(State::WIN):
		FightRender(dTime, batch);
		break;
	case(State::LOSE):
		FightRender(dTime, batch);
		break;
	case(State::SHOP):
		StoreRender(dTime,batch);
		break;
	}
}

void PlayMode::StoreRender(float dtime, SpriteBatch& batch) 
{
	//we need to render all the same things as BuildRender, so...
	BuildRender(dtime, batch);
	//plus extra
	shopSprite.Render(&batch);
}

void PlayMode::BuildRender(float dTime, SpriteBatch& batch) {
	bgSprite.Render(&batch);
	coinSprite.Render(&batch);
	GetFont().DrawString(&batch, to_string(coins).c_str(), Vector2(35, 20));
	grid.RenderGrid(dTime, &batch);
	for (int i = 0; i < gameCreatures.size(); i++) {
		gameCreatures[i].sprite.Render(&batch);
	}
	for (int i = 0; i < uiSprites.size(); i++) {
		uiSprites[i].Render(&batch);
	}
}

void PlayMode::FightRender(float dTime, SpriteBatch& batch)
{

	bgSprite.Render(&batch);
	for (int i = 0; i < gameCreatures.size(); i++) {
		gameCreatures[i].Render(&batch);
	}
	for (int i = 0; i < uiSprites.size(); i++) {
		uiSprites[i].Render(&batch);
	}
	if (state == State::WIN || state == State::LOSE) {
		logoSprite.Render(&batch);
	}
}

void PlayMode::spawnEnemy(creatureType enemyToSpawn, Vector2 position)
{

	position.x += grid.width;

	if (enemyToSpawn == BRELOOM) {
		Creature ebreloom("Breloom", "breloomIdle.dds", Game::Get().GetD3D(), true);
		ebreloom.SpriteInit(grid, position, Vector2(3, 3), false, RECT{ 0,96,40,144 }, RECT{ 0,0,40,48 }, 12, 0.1f);
		gameCreatures.push_back(ebreloom);
	}
	if (enemyToSpawn == BUIZEL){
		Creature buizel(creatureType::BUIZEL, position, grid,true);
		buizel.attackRange = 50.f;
		gameCreatures.push_back(buizel);
}
	if (enemyToSpawn == SKITTY){
			Creature eSkitty("Skitty", "skittyIdle.dds", Game::Get().GetD3D(),true);
			eSkitty.SpriteInit(grid, position, Vector2(3, 3), true, RECT{ 0,80,32,120 }, RECT{ 0,0,32,40 }, 4, 0.4f);
			eSkitty.attackRange = 200.f;
			gameCreatures.push_back(eSkitty);
		}

}


void PlayMode::dragSprite(Sprite& sprite,Mouse& mouse) {
	sprite.setPos(mouse.GetMousePos(true));
}

bool isSpriteClicked(Sprite& sprite, Mouse& mouse) {
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
			if (sprite.type == Sprite::spriteTYPE::UI) {
				sprite.setColour(Colors::DarkGray);
			}
		}
		return false;
	}
}

bool isSpriteClickReleased(Sprite& sprite,Mouse& mouse)
{
		Vector2 mousepos = mouse.GetMousePos(true);
		RECT sRect = sprite.getDim();
		SimpleMath::Rectangle cRect = SimpleMath::Rectangle(sRect);
		cRect.x = sprite.Position.x;
		cRect.y = sprite.Position.y;

		if (cRect.Contains(mousepos))
		{
			if (mouse.isClickRelease()&&!mouse.clickedLastFrame) {
				//pressed
				mouse.clickedLastFrame = true;
				return true;
			}
			else {
				//is hovering over sprite
				sprite.isHover = true;
				return false;
			}
		}
		if (sprite.isHover)
			sprite.isHover = false;
	return false;
}

bool PlayMode::isGridClicked(Grid& Grid, Sprite& sprite, Mouse& mouse) {
	Vector2 gridPos = getGridPosition(grid, sprite.Position);

	//has player clicked within the grid?
	if (Game::Get().mouse.isClickRelease() && getGridPosition(grid, Game::Get().mouse.GetMousePos(true)) != Vector2(420, 420))
	{
		if (gridPos == getGridPosition(grid, mouse.GetMousePos(true)))
		{
			if (sprite.setGridPosition(grid, gridPos.x, gridPos.y))
				return true;
		}
	}
	return false;
}


void PlayMode::UIAction(UISprite& sprite)
{
	if (sprite.canBeClicked) {
		if (sprite.uiType == UISprite::UITYPE::start)
		{
			//reset counters
			enemiesAlive = 0;
			teamAlive = 0;
			//check how many creatures on each team
			for (int i = 0; i < gameCreatures.size(); i++)
			{
				if (gameCreatures[i].isEnemy)
					enemiesAlive++;
				else
					teamAlive++;
			}
			InitBattle();
		}
		else if (sprite.uiType == UISprite::UITYPE::quit)
		{
			PostQuitMessage(0);
		}
		else if (sprite.uiType == UISprite::UITYPE::next)
		{
			ResetBoard();
			InitBuild();
		}
		else if (sprite.uiType == UISprite::UITYPE::restart)
		{
			MyD3D& tempd3d = Game::Get().GetD3D();
			delete& Game::Get();
			new Game(tempd3d);
			Game::Get().GetModeMgr().SwitchMode(GAMEMODE::PLAY);
		}
		else if (sprite.uiType == UISprite::UITYPE::menu)
		{
			Game::Get().GetModeMgr().SwitchMode(GAMEMODE::MENU);
		}
		else if (sprite.uiType == UISprite::UITYPE::store)
		{
			if (state == State::SHOP) {
				state = State::BUILD;
			}
			else {
				InitShop();
			}
		}
	}
}

void PlayMode::ResetBoard() 
{
	//clear enemies
	for (auto it = gameCreatures.begin(); it != gameCreatures.end();) {
		if ((*it).isEnemy)
		{
			it = gameCreatures.erase(it);
		}
		else
			++it;
	}
	//reset grid
	grid.ResetTiles();
	//reset team members
	for (auto it = gameCreatures.begin(); it != gameCreatures.end(); ++it)
	{
		(*it).ResetCreature();
		(*it).sprite.setGridPosition(grid, (*it).sprite.previousGridPos.x, (*it).sprite.previousGridPos.y, false);
	}
	uiSprites.clear();
}
