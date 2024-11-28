#include "creature.h"
#include "sprite.h"
#include "game.h"

void Creature::SpriteInit( Grid& grid, Vector2 position, Vector2 scale, bool centerOrigin, RECT spriterect, RECT framerect, int totalframes, float animspeed) {
	sprite.setGridPosition(grid, position.x, position.y);
	sprite.Scale = scale;
	sprite.spriteRect = spriterect;
	sprite.frameSize = framerect;
	sprite.totalFrames = totalframes;
	sprite.animSpeed = animspeed;
	sprite.isAnim = true;
	sprite.dim = (RECT{ long(sprite.spriteRect.left * sprite.Scale.x), long(sprite.spriteRect.top * sprite.Scale.y), long(sprite.spriteRect.right * sprite.Scale.x), long(sprite.spriteRect.bottom * sprite.Scale.y) });


	if (type==BUIZEL)
	{
		Sprite _walk("Buizel", "buizelIdle.dds",Game::Get().GetD3D());
	}

	lastPos = sprite.Position;
}

Creature::Creature(creatureType typeToMake, Vector2 gridPos, Grid& grid, bool Enemy):isEnemy(Enemy),type(typeToMake)
{
	if (typeToMake == creatureType::BRELOOM) 
	{
		Sprite _sprite("Breloom", "breloomIdle.dds", Game::Get().GetD3D());
		sprite = _sprite;
		SpriteInit(grid, gridPos, Vector2(3, 3), false, RECT{ 0,96,40,144 }, RECT{ 0,0,40,48 }, 12, 0.1f);
		//set other animations
		Sprite wSprite("breloomWalk", "breloomWalk.dds", Game::Get().GetD3D());
		walkSprite = wSprite;
		walkSprite.Init(gridPos, Vector2(3, 3), true, RECT{ 0,96,40,144 }, RECT{ 0,0,40,48 }, 4, 0.1f);
	}
	if (typeToMake == creatureType::BUIZEL)
	{
		Sprite _sprite("Buizel", "buizelIdle.dds", Game::Get().GetD3D());
		sprite = _sprite;
		SpriteInit(grid, gridPos, Vector2(3, 3), true, RECT{ 0,288,32,336 }, RECT{ 0,0,32,48 }, 9, 0.2f);
		//set other animations
		Sprite wSprite("buizelWalk", "buizelWalk.dds", Game::Get().GetD3D());
		walkSprite = wSprite;
		walkSprite.Init(gridPos, Vector2(3, 3), true, RECT{ 0,240,32,280 }, RECT{ 0,0,32,40 }, 4, 0.2f);
	}
	if (typeToMake == creatureType::SKITTY) 
	{
		Sprite _sprite("Skitty", "skittyIdle.dds", Game::Get().GetD3D());
		sprite = _sprite;
		SpriteInit(grid, gridPos, Vector2(3, 3), true, RECT{ 0,80,32,120 }, RECT{ 0,0,32,40 }, 4, 0.4f);
		attackRange = 200.f;
		//set other animations
		Sprite wSprite("skittyWalk", "skittyWalk.dds", Game::Get().GetD3D());
		walkSprite = wSprite;
		walkSprite.Init(gridPos, Vector2(3, 3), true, RECT{ 0,96,40,144 }, RECT{ 0,0,40,48 }, 7, 0.2f);
	}

	idleSprite = sprite;
}

void Creature::ChangeAnimation(ACTION toChangeTo)
{
	switch(toChangeTo) 
	{
	case WALK:
		walkSprite.setPos(sprite.Position);
		sprite = walkSprite;
		break;
	case IDLE:
		idleSprite.setPos(sprite.Position);
		sprite = idleSprite;
		break;
	}


	currAction = toChangeTo;
}

void Creature::Update(float dTime) 
{
	if (lastPos != sprite.Position && currAction != ACTION::WALK)
		ChangeAnimation(ACTION::WALK);
	else if (currAction!=IDLE && lastPos == sprite.Position)
		ChangeAnimation(ACTION::IDLE);
	sprite.Update(dTime);
	lastPos = sprite.Position;
}