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
	enemyGrid = _grid;
	enemyGrid.visible = false;
	enemyGrid.width * 2;

	//Creature breloom("Breloom", "breloomIdle.dds", Game::Get().GetD3D());
	//breloom.SpriteInit(grid, Vector2(0, 5), Vector2(3, 3), false, RECT{ 0,96,40,144 }, RECT{ 0,0,40,48 }, 12, 0.1f);
	Creature breloom(creatureType::BRELOOM, Vector2(0, 5), grid);
	gameCreatures.push_back(breloom);

	Creature skitty(creatureType::SKITTY, Vector2(2, 5), grid);
	gameCreatures.push_back(skitty);

	spawnEnemy(BUIZEL,Vector2(2,4));
	spawnEnemy(BUIZEL, Vector2(1, 1));

	spriteDragging = false;


	Sprite Button("Button", "startButton.dds", Game::Get().GetD3D());
	Button.Init(Vector2(430, 50), Vector2(0.15, 0.15), Vector2(0, 0), RECT{ 0,0,1280,427 });
	Button.type = Sprite::spriteTYPE::UI;
	uiSprites.push_back(Button);
}

void PlayMode::BuildUpdate(float dTime)
{
	Mouse& mouse = Game::Get().mouse;
	//UPDATE UI
	for (int i = 0; i < uiSprites.size(); i++)
	{
		uiSprites[i].Update(dTime);
		if (isSpriteClickReleased(uiSprites[i],mouse) && !spriteDragging)
		{
			InitBattle();
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

void PlayMode::FightUpdate(float dTime)
{
	for (int i = 0; i < gameCreatures.size(); i++) 
	{
		gameCreatures[i].Update(dTime,true);
		gameCreatures[i].targetIndex = findClosest(i, gameCreatures[i].isEnemy); //find closest potential target

		if(!checkCol(gameCreatures[i], gameCreatures[gameCreatures[i].targetIndex])) //if the creature isn't already within attack range of it's target
		{
			gameCreatures[i].sprite.setPos(MoveTowards(gameCreatures[i].sprite.Position,
			gameCreatures[gameCreatures[i].targetIndex].sprite.Position,
			gameCreatures[i].speed)); 
		}
		else
		{
			if (gameCreatures[i].readyToAttack)
				gameCreatures[i].Attack(gameCreatures[gameCreatures[i].targetIndex]);
		}
	}
}

void PlayMode::InitBattle()
{
	for (int i = 0; i < uiSprites.size(); i++)
	{
		uiSprites[i].active = false;
	}


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
	}
}

void PlayMode::BuildRender(float dTime, SpriteBatch& batch) {
	bgSprite.Render(&batch);
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
		gameCreatures[i].sprite.Render(&batch);
	}
	for (int i = 0; i < uiSprites.size(); i++) {
		uiSprites[i].Render(&batch);
	}
}

void PlayMode::spawnEnemy(creatureType enemyToSpawn, Vector2 position)
{

	position.x += grid.width;

	if (enemyToSpawn == BRELOOM) {
		Creature ebreloom("Breloom", "breloomIdle.dds", Game::Get().GetD3D(), true);
		ebreloom.SpriteInit(enemyGrid, position, Vector2(3, 3), false, RECT{ 0,96,40,144 }, RECT{ 0,0,40,48 }, 12, 0.1f);
		gameCreatures.push_back(ebreloom);
	}
	if (enemyToSpawn == BUIZEL){
		Creature buizel(creatureType::BUIZEL, position, enemyGrid,true);
		buizel.attackRange = 50.f;
		gameCreatures.push_back(buizel);
}
	if (enemyToSpawn == SKITTY){
			Creature eSkitty("Skitty", "skittyIdle.dds", Game::Get().GetD3D(),true);
			eSkitty.SpriteInit(enemyGrid, position, Vector2(3, 3), true, RECT{ 0,80,32,120 }, RECT{ 0,0,32,40 }, 4, 0.4f);
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
				sprite.setColour(Colors::DarkGreen);
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
		if (mouse.isClickRelease()) {
			//pressed
			return true;
		}
		else {
			//is hovering over sprite
			if (sprite.type == Sprite::spriteTYPE::UI) {
				sprite.setColour(Colors::DarkGreen);
			}
		}
		return false;
	}
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

