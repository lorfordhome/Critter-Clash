#pragma once
#include <string>
#include <cassert>
#include "sprite.h"
#include "game.h"
#include "LuaHelper.h"


Vector2 MoveTowards(raylib::Vector2 current, raylib::Vector2 target, float maxDistanceDelta)
{
	raylib::Vector2 a = target - current;
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
	//reserving space for vectors now as resizing during runtime is costly
	gameCreatures.reserve(10);
	shopCreatures.reserve(4);
	uiSprites.reserve(4);
	playedTroops.resize(5);

	srand(GetTickCount64());

	Sprite _bg("Background", "map6.dds", Game::Get().GetD3D()); //temp for initialising
	_bg.Init(Vector2(0, 0), Vector2(2, 2), Vector2(0, 0), RECT{ 0,0,512,384 });
	bgSprite = _bg;

	Grid _grid(Game::Get().GetD3D()); 
	grid = _grid;

	Sprite _coin("coin", "coin.dds", Game::Get().GetD3D());
	_coin.Init(Vector2(5, 20), Vector2(0.8, 0.8), Vector2(0, 0), RECT{ 0,0,45,48 });
	coinSprite = _coin;

	Sprite _sprite("store", "store.dds", Game::Get().GetD3D());
	_sprite.Init(Vector2(525, 0), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,512,768 });
	shopSprite = _sprite;


	pixelFont = new SpriteFont(&Game::Get().GetD3D().GetDevice(), L"../bin/data/pixelText.spritefont");
	assert(pixelFont);

	pixelFontSmall = new SpriteFont(&Game::Get().GetD3D().GetDevice(), L"../bin/data/pixelTextSmall.spritefont");
	assert(pixelFontSmall);
	L = luaL_newstate();
	luaL_openlibs(L);

	InitLuaFunctions(disp);
	InitBuild();


}

void PlayMode::InitShop() 
{
	if (resetShop) //only reset shop between rounds
	{
		shopCreatures.clear();
		SpawnShopCreatures();
		SetShopPositions();
		resetShop = false;
	}

	UISprite _box("sellBox", "sellBox.dds", Game::Get().GetD3D());
	_box.Init(Vector2(575, 590), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,416,146 });
	_box.type = Sprite::spriteTYPE::UI;
	_box.uiType = UISprite::UITYPE::sell;

	uiSprites.push_back(_box);

	state = State::SHOP;
}

void PlayMode::SetShopPositions() 
{
	Vector2 Pos = { baseTilePos.x + shopCreatureOffsetX,baseTilePos.y+shopCreatureOffsetY };
	for (int i = 0; i < shopCreatures.size(); ++i) {
		//messy solution - fix later?
		if (i == 0)
			shopCreatures[i].sprite.setPos(Pos);
		else if (i == 1)
			shopCreatures[i].sprite.setPos(Vector2(Pos.x + pixelsBetweenTilesX, Pos.y));
		else if (i == 2)
			shopCreatures[i].sprite.setPos(Vector2(Pos.x, Pos.y + pixelsBetweenTilesY));
		else if (i == 3)
			shopCreatures[i].sprite.setPos(Vector2(Pos.x + pixelsBetweenTilesX, Pos.y + pixelsBetweenTilesY));
		shopCreatures[i].lastPos = shopCreatures[i].sprite.Position;
	}
}

void PlayMode::SpawnShopCreatures() 
{
	for (int i = 0; i < maxShopSlots; ++i) {
		int rand = std::rand() % 3; //generate number from 1 to 3

		if (rand == 0) {
			//spawn breloom
			Creature breloom(creatureType::BRELOOM, Vector2(0, 0), grid,true,false);
			shopCreatures.push_back(breloom);
		}
		if (rand == 1) {
			//spawn buizel
			Creature buizel(creatureType::BUIZEL, Vector2(0, 0), grid,true,false);
			shopCreatures.push_back(buizel);
		}
		if (rand == 2) {
			//spawn skitty
			Creature skitty(creatureType::SKITTY, Vector2(0, 0), grid,true,false);
			shopCreatures.push_back(skitty);
		}
	}
}

void PlayMode::GenerateEnemies() //generate all enemies
{
	int desiredDifficulty = currentRound / 2;
	desiredDifficulty -= 1;
	if (desiredDifficulty < 0)
		desiredDifficulty = 0;
	if (desiredDifficulty > 4)
		desiredDifficulty = 4;
	//if there are no valid saved troops to fight, randomly generate
	if (Game::Get().troopCounts.empty() || Game::Get().troopCounts[desiredDifficulty] == 0 || playedTroops[desiredDifficulty].size() >= Game::Get().troopCounts[desiredDifficulty]) {
		while (enemiesAlive < (currentRound + 1)) //will run until number of enemies reaches desired amount
		{
			GenerateRandomEnemy();
			if (enemiesAlive >= grid.gridHeight * grid.gridWidth) //if the number of enemies has exceeded the grid capacity, stop
				return;
		}
		return;
	}
	//since there are valid troops  we can fight, generate them from the script
	GenerateScriptEnemies(desiredDifficulty);
}

void PlayMode::GenerateScriptEnemies(int difficulty)
{
	//generate number, from which the troop will be selected
	int rand = std::rand() % Game::Get().troopCounts[difficulty];

	//if we've fought at this difficulty before, make sure you don't repeat a fight
	if (!playedTroops[difficulty].empty()) {
		bool uniqueTroop = false;
		while (!uniqueTroop) {
			rand = std::rand() % Game::Get().troopCounts[difficulty];
			for (auto it = playedTroops[difficulty].begin(); it != playedTroops[difficulty].end(); ++it) {
				if (rand == *it) {
					uniqueTroop = false;
					break;
				}
				else {
					uniqueTroop = true;
				}
			}
		}
	}

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	Execute(L, "Difficulty" + to_string(difficulty+1) + ".lua");
	int creatureCount = Get2DTableLength(L, "Troops", rand+1);
	for (int i = 0; i < creatureCount; ++i) {
		creatureDetails creatureToSpawn;
		creatureToSpawn.fromLua(L, rand+1, i + 1);
		spawnEnemy(creatureToSpawn.type, creatureToSpawn.position);
	}
	playedTroops[difficulty].push_back(rand);
	lua_close(L);
}

void PlayMode::GenerateRandomEnemy() 
{
	int rand2 = std::rand() % 3; //randomly generate x pos to place enemy in
	int rand3 = std::rand() % 4;//randomly generate y pos to place enemy in


	for (auto it = gameCreatures.begin(); it != gameCreatures.end();++it) {
		if ((*it).isEnemy) {
			if (getGridPosition((grid.gridWidth*2),grid.gridHeight,grid.cellSize,grid.gridOriginX,grid.gridOriginY, (*it).sprite.Position) == Vector2(rand2+grid.gridWidth, rand3) )
			{
				return; //can't spawn enemy - will overlap
			}
		}
	}
	//if it didn't return, a valid position was created - now we can spawn the enemy

	int rand = std::rand() % 3; //randomly generate creature
	if (rand == 0) {
		//spawn breloom
		spawnEnemy(BRELOOM, Vector2(rand2, rand3));
	}
	if (rand == 1) {
		//spawn buizel
		spawnEnemy(BUIZEL, Vector2(rand2, rand3));
	}
	if (rand == 2) {
		//spawn skitty
		spawnEnemy(SKITTY, Vector2(rand2, rand3));
	}
	enemiesAlive++;
}

void PlayMode::InitBuild() 
{

	//check if music is playing - if no, load music
	if (!Game::Get().getAudioMgr().GetSongMgr()->IsPlaying(Game::Get().musicHdl)) {
		Game::Get().getAudioMgr().GetSongMgr()->Play(utf8string("MenuMusic"), true, false, &Game::Get().musicHdl, Game::Get().getAudioMgr().GetSongMgr()->GetVolume());
	}
	//reset bools
	resetShop = true;
	spriteDragging = false;

	GenerateEnemies();
	
	//init ui
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
void PlayMode::InitLose()
{
	state = State::LOSE;
	//init text
	Sprite Logo("defeatText", "defeatText.dds", Game::Get().GetD3D());
	Logo.Init(Vector2(350, 50), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,313,73 });
	logoSprite = Logo;
	//init ui
	UISprite Button3("homeButton", "homeButton.dds", Game::Get().GetD3D());
	Button3.Init(Vector2(410, 300), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,144,72 });
	Button3.type = Sprite::spriteTYPE::UI;
	Button3.uiType = UISprite::UITYPE::menu;
	uiSprites.push_back(Button3);

	UISprite Button("restartButton", "restartButton.dds", Game::Get().GetD3D());
	Button.Init(Vector2(410, 400), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,144,72 });
	Button.type = Sprite::spriteTYPE::UI;
	Button.uiType = UISprite::UITYPE::restart;
	uiSprites.push_back(Button);
	if (Game::Get().getAudioMgr().GetSongMgr()->IsPlaying(Game::Get().musicHdl)) {
		Game::Get().getAudioMgr().GetSongMgr()->Stop();
	}
	Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("Defeat"), false, false, &Game::Get().sfxHdl);
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
	if (Game::Get().getAudioMgr().GetSongMgr()->IsPlaying(Game::Get().musicHdl)) {
		Game::Get().getAudioMgr().GetSongMgr()->Stop();
	}
	Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("Victory"), false, false, &Game::Get().sfxHdl);

}

void PlayMode::InitBattle()
{
	//music
	if (Game::Get().getAudioMgr().GetSongMgr()->IsPlaying(Game::Get().musicHdl)) {
		Game::Get().getAudioMgr().GetSongMgr()->Stop();
		Game::Get().getAudioMgr().GetSongMgr()->Play(utf8string("BattleMusic"), true, false, &Game::Get().musicHdl, Game::Get().getAudioMgr().GetSongMgr()->GetVolume());
	}

	//store grid position before fight
	for (int i = 0; i < gameCreatures.size(); ++i) {
		if (!gameCreatures[i].isEnemy)
			gameCreatures[i].idleSprite.previousGridPos = getGridPosition(grid, gameCreatures[i].sprite.Position);
	}
	uiSprites.clear();
	state = State::FIGHT;
}

void PlayMode::BuildUpdate(float dTime)
{
	//make sure music is playing
	if (!Game::Get().getAudioMgr().GetSongMgr()->IsPlaying(Game::Get().musicHdl)) {
		Game::Get().getAudioMgr().GetSongMgr()->Play(utf8string("MenuMusic"), true, false, &Game::Get().musicHdl, Game::Get().getAudioMgr().GetSongMgr()->GetVolume());
	}
	//pull mouse now to save on performance
	Mouse& _mouse = Game::Get().mouse;
	//UPDATE UI
	for (int i = 0; i < uiSprites.size(); i++)
	{
		uiSprites[i].Update(dTime);
		if (isSpriteClickReleased(uiSprites[i], _mouse) && !spriteDragging)
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
			if (isSpriteClicked(gameCreatures[i].sprite, _mouse) && !spriteDragging && !gameCreatures[i].isEnemy) {
				gameCreatures[i].sprite.previousGridPos = getGridPosition(grid, gameCreatures[i].sprite.Position);//store grid position in case it needs to be reset
				spriteDragging = true;
				movedSprite = i; //store the sprite that is being moved
			}
		}
	}
	//is player currently trying to move a sprite?
	if (spriteDragging) {
		dragSprite(gameCreatures[movedSprite].sprite, _mouse);
		if (_mouse.isClickRelease()) {
			if (isGridClicked(grid, gameCreatures[movedSprite].sprite, _mouse))
			{
				//pressed
				spriteDragging = false;
				PlaceCreatureSFX(gameCreatures[movedSprite]);
			}
			else {
				//user tried to place sprite in invalid position - reset the sprite to original pos
				gameCreatures[movedSprite].sprite.setGridPosition(grid, gameCreatures[movedSprite].sprite.previousGridPos.x, gameCreatures[movedSprite].sprite.previousGridPos.y, false);
				spriteDragging = false;
			}
		}

	}
}

void PlayMode::PlaceCreatureSFX(Creature& creature) 
{
	switch (creature.type) {
	case BUIZEL:
		Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("BuizelHi"), false, false, &Game::Get().sfxHdl);
		break;
	case BRELOOM:
		Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("BreloomHi"), false, false, &Game::Get().sfxHdl);
		break;
	case SKITTY:
		Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("SkittyHi"), false, false, &Game::Get().sfxHdl);
		break;
	}
}

void PlayMode::StoreUpdate(float dTime) 
{
	//pull mouse now to save on performance
	Mouse& _mouse = Game::Get().mouse;
	//UPDATE UI
	for (int i = 0; i < uiSprites.size(); i++)
	{
		if (uiSprites[i].uiType != UISprite::start) //we dont want the play button to function while we're in the shop menu
		{
			uiSprites[i].Update(dTime);
			if (isSpriteClickReleased(uiSprites[i], _mouse) && ((!spriteDragging&&uiSprites[i].uiType != UISprite::sell)||(uiSprites[i].uiType==UISprite::sell&&!draggingShop&&spriteDragging)))
			{
				if (uiSprites[i].Action()) {
					UIAction(uiSprites[i]);
					if (uiSprites.size() > 0) {
						uiSprites[i].canBeClicked = false;
					}
				}
			}
		}
	}

	for (int i = 0; i < gameCreatures.size(); i++) {
		gameCreatures[i].sprite.Update(dTime);
		if (!spriteDragging) {
			//has player clicked a sprite?
			if (isSpriteClicked(gameCreatures[i].sprite, _mouse) && !spriteDragging && !gameCreatures[i].isEnemy) {
				gameCreatures[i].sprite.previousGridPos = getGridPosition(grid, gameCreatures[i].sprite.Position);//store grid position in case it needs to be reset
				spriteDragging = true;
				movedSprite = i; //store the sprite that is being moved
			}
		}
	}

	for (int i = 0; i < shopCreatures.size(); ++i) 
	{
		shopCreatures[i].Update(dTime, false, true);
		if (!spriteDragging) {
			//has player clicked a sprite?
			if (isSpriteClicked(shopCreatures[i].sprite, _mouse) && !spriteDragging) 
			{
				if (coins < shopCreatures[i].getValue()) //if player can't afford, flash red to indicate it
				{
					shopCreatures[i].TakeDamage(0);
				}
				else 
				{
					spriteDragging = true;
					movedSprite = i; //store the sprite that is being moved
					draggingShop = true;
				}
			}
		}
	}

	//is player currently trying to move a sprite?
	if (spriteDragging) {

		//check if they're dragging a shop creature
		if (draggingShop){
			dragSprite(shopCreatures[movedSprite].sprite, _mouse);
			if (_mouse.isClickRelease()) {
				if (isGridClicked(grid, shopCreatures[movedSprite].sprite, _mouse, true))
				{

					//player placed shop creature into their team
					PlaceCreatureSFX(shopCreatures[movedSprite]);
					spriteDragging = false;
					draggingShop = false;
					coins -= shopCreatures[movedSprite].getValue();
					gameCreatures.push_back(shopCreatures[movedSprite]);
					shopCreatures.erase(shopCreatures.begin() + movedSprite);
					//readjust shop order
					SetShopPositions();

				}
				else {
					//user tried to place sprite in invalid position - reset the sprite to original pos
					shopCreatures[movedSprite].sprite.setPos(shopCreatures[movedSprite].lastPos);
					spriteDragging = false;
					draggingShop = false;
				}
			}
		}
		else //player is dragging team creature
		{
			dragSprite(gameCreatures[movedSprite].sprite, _mouse);
			if (_mouse.isClickRelease()) {
				if (isGridClicked(grid, gameCreatures[movedSprite].sprite, _mouse))
				{
					//pressed
					spriteDragging = false;
					PlaceCreatureSFX(gameCreatures[movedSprite]);
				}
				else {
					//user tried to place sprite in invalid position - reset the sprite to original pos
					gameCreatures[movedSprite].sprite.setGridPosition(grid, gameCreatures[movedSprite].sprite.previousGridPos.x, gameCreatures[movedSprite].sprite.previousGridPos.y, false);
					spriteDragging = false;
				}
			}
		}

	}

}

void PlayMode::FightUpdate(float dTime)
{
	for (int i = 0; i < gameCreatures.size(); i++) 
	{
		if (gameCreatures[i].active) 
		{
			//update returns false if the creature has died
			if (!gameCreatures[i].Update(dTime, true))
			{
				if (gameCreatures[i].isEnemy) {
					enemiesAlive--;
				}
				else
					teamAlive--;
			}
			gameCreatures[i].targetIndex = findClosest(i, gameCreatures[i].isEnemy); //find closest potential target

			if (!checkCol(gameCreatures[i], gameCreatures[gameCreatures[i].targetIndex])) //if the creature isn't already within attack range of it's target
			{
				Vector2 posToMove = MoveTowards(gameCreatures[i].sprite.Position,
					gameCreatures[gameCreatures[i].targetIndex].sprite.Position,
					gameCreatures[i].speed*dTime);
				gameCreatures[i].sprite.setPos(posToMove);
			}
			else
			{
				if (gameCreatures[i].readyToAttack)
					gameCreatures[i].Attack(gameCreatures[gameCreatures[i].targetIndex]);
			}
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

	Mouse& _mouse = Game::Get().mouse;
	//UPDATE UI
	for (int i = 0; i < uiSprites.size(); i++)
	{
		uiSprites[i].Update(dTime);
		if (isSpriteClickReleased(uiSprites[i], _mouse) && !spriteDragging)
		{
			UIAction(uiSprites[i]);
		}
	}
	if(flagRestart)
		Game::Get().RestartGame();
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

void PlayMode::BuildRender(float dTime, SpriteBatch& batch) {
	bgSprite.Render(&batch);
	coinSprite.Render(&batch);
	GetFont().DrawString(&batch, to_string(coins).c_str(), Vector2(35, 20));
	grid.RenderGrid(dTime, &batch);
	for (int i = 0; i < uiSprites.size(); i++) {
		uiSprites[i].Render(&batch);
	}
	for (int i = 0; i < gameCreatures.size(); i++) {
		gameCreatures[i].sprite.Render(&batch);
	}
	if (hasSavedTeam) {
		GetFontSmall().DrawString(&batch, "Successfully Saved Team To File", Vector2(300, 20));
		textTimer += dTime;
		if (textTimer >= timeToDisplaySaveText) {
			hasSavedTeam = false;
			textTimer = 0.f;
		}
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
	if (state == State::LOSE) 
	{
		GetFont().DrawString(&batch, ("You got to Round " + to_string(currentRound)).c_str(), Vector2(265, 150));
	}
}

void PlayMode::spawnEnemy(creatureType enemyToSpawn, Vector2 position)
{

	position.x += grid.gridWidth;

	if (enemyToSpawn == BRELOOM) {
		Creature ebreloom(creatureType::BRELOOM, position, grid, false, true);
		gameCreatures.push_back(ebreloom);
	}
	if (enemyToSpawn == BUIZEL){
		Creature buizel(creatureType::BUIZEL, position, grid,false,true);
		gameCreatures.push_back(buizel);
}
	if (enemyToSpawn == SKITTY){
		Creature eSkitty(creatureType::SKITTY, position, grid, false, true);
		gameCreatures.push_back(eSkitty);
		}


}

void PlayMode::StoreRender(float dTime, SpriteBatch& batch)
{
	bgSprite.Render(&batch);
	coinSprite.Render(&batch);
	GetFont().DrawString(&batch, to_string(coins).c_str(), Vector2(35, 20));
	grid.RenderGrid(dTime, &batch);
	shopSprite.Render(&batch);
	for (int i = 0; i < uiSprites.size(); i++) {
		//only need store button to render
		if (uiSprites[i].uiType != UISprite::start)
			uiSprites[i].Render(&batch);
	}

	coinSprite.setScale(Vector2(0.25, 0.25));  //set coinSprite for correct values to be rendered in shop
	//messy solution - fix later
	for (int i = 0; i < shopCreatures.size(); i++) 
	{
		if (i == 0)
			RenderShopTile(shopCreatures[i], baseTilePos, batch);
		if (i == 1)
			RenderShopTile(shopCreatures[i], Vector2(baseTilePos.x + pixelsBetweenTilesX, baseTilePos.y),batch);
		if (i == 2)
			RenderShopTile(shopCreatures[i], Vector2(baseTilePos.x, baseTilePos.y + pixelsBetweenTilesY),batch);
		if (i == 3)
			RenderShopTile(shopCreatures[i], Vector2(baseTilePos.x + pixelsBetweenTilesX, baseTilePos.y + pixelsBetweenTilesY),batch);
	}

	//reset coinsprite 
	coinSprite.setScale(Vector2(0.8, 0.8)); 
	coinSprite.setPos(Vector2(5, 20));

	for (int i = 0; i < gameCreatures.size(); i++) {
		//we don't want to render enemy creatures as they will overlap with the shop window
		if (!gameCreatures[i].isEnemy)
			gameCreatures[i].sprite.Render(&batch);
	}

}


void PlayMode::RenderShopTile(Creature& creature,Vector2 tilePosition,SpriteBatch& batch) //pass in upper-right corner of tile
{
	//need to fix these magic numbers
	GetFontSmall().DrawString(&batch, creature.getName(), Vector2(tilePosition.x+70,tilePosition.y+5));
	GetFontSmall().DrawString(&batch, creature.getDescriptor(), Vector2(tilePosition.x+30, tilePosition.y + 150));
	GetFontSmall().DrawString(&batch, to_string(creature.getValue()).c_str(), Vector2(tilePosition.x+15, tilePosition.y+5));
	coinSprite.setPos(Vector2(tilePosition.x + 5, tilePosition.y + 8));
	coinSprite.Render(&batch);
	creature.sprite.Render(&batch);

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
	return false;
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

bool PlayMode::isGridClicked(Grid& Grid, Sprite& sprite, Mouse& mouse, bool noPrev) {
	Vector2 gridPos = getGridPosition(grid, sprite.Position);

	//has player clicked within the grid?
	if (Game::Get().mouse.isClickRelease() && getGridPosition(grid, Game::Get().mouse.GetMousePos(true)) != Vector2(420, 420))
	{
		if (!noPrev) {
			if (gridPos == getGridPosition(grid, mouse.GetMousePos(true)))
			{
				if (sprite.setGridPosition(grid, gridPos.x, gridPos.y))
					return true;
			}
		}
		else
		{
			if (gridPos == getGridPosition(grid, mouse.GetMousePos(true)))
			{
				if (sprite.setGridPositionNoPrev(grid, gridPos.x, gridPos.y))
					return true;
			}
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
			if (teamAlive <= 0)
				return;
			InitBattle();
		}
		else if (sprite.uiType == UISprite::UITYPE::quit)
		{
			PostQuitMessage(0);
		}
		else if (sprite.uiType == UISprite::UITYPE::next)
		{
			ResetBoard();
			currentRound++;
			coins += 10 + (rand() % 10); //gain random amount of coins, minimum 10
			Game::Get().getAudioMgr().GetSongMgr()->Stop();
			InitBuild();
		}
		else if (sprite.uiType == UISprite::UITYPE::restart)
		{
			flagRestart = true;
		}
		else if (sprite.uiType == UISprite::UITYPE::menu)
		{
			Game::Get().GetModeMgr().DeleteMode(GAMEMODE::PLAY);
			Game::Get().GetModeMgr().AddMode(new PlayMode());
			Game::Get().GetModeMgr().SwitchMode(GAMEMODE::MENU);
			//reset music
			if (Game::Get().getAudioMgr().GetSongMgr()->IsPlaying(Game::Get().musicHdl)) {
				Game::Get().getAudioMgr().GetSongMgr()->Stop();
				Game::Get().getAudioMgr().GetSongMgr()->Play(utf8string("MenuMusic"), true, false, &Game::Get().musicHdl, Game::Get().getAudioMgr().GetSongMgr()->GetVolume());
			}
		}
		else if (sprite.uiType == UISprite::UITYPE::store)
		{
			if (state == State::SHOP) 
			{
				state = State::BUILD;

				//remove sell box sprite
				for (auto it = uiSprites.begin(); it != uiSprites.end();) {
					if ((*it).uiType==UISprite::sell)
					{
						it = uiSprites.erase(it);
					}
					else
						++it;
				}
			}
			else {
				InitShop();
			}
		}
		else if (sprite.uiType == UISprite::sell) 
		{
			Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("sell"), false, false, &Game::Get().sfxHdl);
			grid.updateTile(gameCreatures[movedSprite].sprite.previousGridPos.x, gameCreatures[movedSprite].sprite.previousGridPos.y, Tile::NONE);
			coins += gameCreatures[movedSprite].getValue()/2;
			gameCreatures.erase(gameCreatures.begin() + movedSprite);
			spriteDragging = false;
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
		(*it).sprite.setGridPositionNoPrev(grid, (*it).sprite.previousGridPos.x, (*it).sprite.previousGridPos.y);
	}
	enemiesAlive = 0;
	teamAlive = 0;
	shopCreatures.clear();
	uiSprites.clear();
}

void PlayMode::InitLuaFunctions(Dispatcher& disp) {
	Execute(L, "Script.lua");
	//initialising dispatcher here
	disp.Init(L);
	//tell dispatcher we have a function for lua
	Dispatcher::Command::voidvoidfunc f{ [this](void) {return InitWin(); } };
	disp.Register("InitWin", Dispatcher::Command{nullptr, f });
	Dispatcher::Command::voidvoidfunc fn{ [this](void) {return InitLose(); } };
	disp.Register("InitLose", Dispatcher::Command{ nullptr, fn });
}
void PlayMode::ApplyLuaCheats(bool defeat) {
	Execute(L, "Script.lua");
	if (!defeat)
		CallVoidVoidCFunc(L, "InitWin");
	else
		CallVoidVoidCFunc(L, "InitLose");
}