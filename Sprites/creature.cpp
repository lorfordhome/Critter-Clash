#include "creature.h"
#include "sprite.h"
#include "game.h"

void Creature::SpriteInit( Grid& grid, Vector2 position, Vector2 scale, bool centerOrigin, RECT spriterect, RECT framerect, int totalframes, float animspeed, bool isShop) {
	if (!isShop) {
		if (isEnemy)
			sprite.setGridPosition(grid, position.x, position.y, false);
		else
			sprite.setGridPosition(grid, position.x, position.y);
	}
	sprite.Scale = scale;
	sprite.spriteRect = spriterect;
	sprite.frameSize = framerect;
	sprite.totalFrames = totalframes;
	sprite.animSpeed = animspeed;
	sprite.isAnim = true;
	sprite.dim = (RECT{ long(sprite.spriteRect.left * sprite.Scale.x), long(sprite.spriteRect.top * sprite.Scale.y), long(sprite.spriteRect.right * sprite.Scale.x), long(sprite.spriteRect.bottom * sprite.Scale.y) });

	lastPos = sprite.Position;
}


Creature::Creature(creatureType typeToMake, Vector2 gridPos, Grid& grid, bool isShop, bool Enemy):isEnemy(Enemy),type(typeToMake)
{
	if (typeToMake == creatureType::BRELOOM) 
	{
		Sprite _sprite("Breloom", "breloomIdle.dds", Game::Get().GetD3D());
		sprite = _sprite;
		SpriteInit(grid, gridPos, Vector2(3, 3), false, RECT{ 0,96,40,144 }, RECT{ 0,0,40,48 }, 12, 0.1f,isShop);
		//set stats
		maxHealth = 125;
		attackCooldown = 1.15f;
		//set other animations
		Sprite wSprite("breloomWalk", "breloomWalk.dds", Game::Get().GetD3D());
		walkSprite = wSprite;
		walkSprite.Init(gridPos, Vector2(3, 3), true, RECT{ 0,96,40,144 }, RECT{ 0,0,40,48 }, 4, 0.1f);
	}
	if (typeToMake == creatureType::BUIZEL)
	{
		Sprite _sprite("Buizel", "buizelIdle.dds", Game::Get().GetD3D());
		sprite = _sprite;
		SpriteInit(grid, gridPos, Vector2(3, 3), true, RECT{ 0,96,32,144 }, RECT{ 0,0,32,48 }, 9, 0.2f, isShop);
		//set other animations
		Sprite wSprite("buizelWalk", "buizelWalk.dds", Game::Get().GetD3D());
		walkSprite = wSprite;
		walkSprite.Init(gridPos, Vector2(3, 3), true, RECT{ 0,80,32,120 }, RECT{ 0,0,32,40 }, 4, 0.2f);
	}
	if (typeToMake == creatureType::SKITTY) 
	{
		Sprite _sprite("Skitty", "skittyIdle.dds", Game::Get().GetD3D());
		sprite = _sprite;
		SpriteInit(grid, gridPos, Vector2(3, 3), true, RECT{ 0,80,32,120 }, RECT{ 0,0,32,40 }, 4, 0.4f, isShop);
		//set stats
		attackRange = 300.f;
		maxHealth = 75;
		//set other animations
		Sprite wSprite("skittyWalk", "skittyWalk.dds", Game::Get().GetD3D());
		walkSprite = wSprite;
		walkSprite.Init(gridPos, Vector2(3, 3), true, RECT{ 0,96,40,144 }, RECT{ 0,0,40,48 }, 7, 0.2f);
	}
	health = maxHealth;
	idleSprite = sprite;

	//initialise healthbar
	if (isEnemy) {
		Sprite hSprite("enemyHealth", "Enemy Health Bar.dds", Game::Get().GetD3D());
		healthBar = hSprite;
		healthBar.Init(Vector2(0, 0), Vector2(0.35, 0.5), Vector2(0, 0), RECT{ 0,0,277,11 });
		ChangeDirection();
	}
	else {
		Sprite hSprite("playerHealth", "Player Health Bar.dds", Game::Get().GetD3D());
		healthBar = hSprite;
		healthBar.Init(Vector2(0, 0), Vector2(0.35, 0.5), Vector2(0, 0), RECT{ 0,0,277,11 });

	}
}

void Creature::ResetCreature()
{
	health = maxHealth;
	active = true;
	flashing = false;
	readyToAttack = true;
	attackTimer = 0.f;
	flashTimer = 0.f;
	ChangeAnimation(ACTION::IDLE);
	UpdateHealthBar();
	if (facingLeft && !isEnemy)
		ChangeDirection();
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

void Creature::UpdateHealthBar() 
{
	float healthPercentage = health / maxHealth;
	healthPercentage = 0.35 * healthPercentage;
	healthBar.Scale.x = healthPercentage;
}

void Creature::ChangeDirection() 
{
	RECT _rect = idleSprite.getspriteRect();
	RECT _rect2 = walkSprite.getspriteRect();
	if (!facingLeft) 
	{
		idleSprite.setSpriteRect(RECT{ _rect.left,idleSprite.getFrameSize().bottom * 6,_rect.right, idleSprite.getFrameSize().bottom * 7 });
		walkSprite.setSpriteRect(RECT{ _rect2.left,walkSprite.getFrameSize().bottom * 6,_rect2.right, walkSprite.getFrameSize().bottom * 7 });
		facingLeft = true;
	}
	else 
	{
		idleSprite.setSpriteRect(RECT{ _rect.left,idleSprite.getFrameSize().bottom * 2,_rect.right, idleSprite.getFrameSize().bottom * 3 });
		walkSprite.setSpriteRect(RECT{ _rect2.left,walkSprite.getFrameSize().bottom * 2,_rect2.right, walkSprite.getFrameSize().bottom * 3 });
		facingLeft = false;
	}
	ChangeAnimation(currAction);
}

bool Creature::Update(float dTime, bool fightMode, bool isShop) 
{
	if (isShop) 
	{
		sprite.Update(dTime);
		//damage flash
		if (flashing)
		{
			flashTimer += dTime;
			if (flashTimer >= damageFlashDuration) {
				sprite.colour = Colours::White;
				flashing = false;
			}
		}
		return true;
	}
	if (active) {
		if (lastPos != sprite.Position && currAction != ACTION::WALK)
		{
			ChangeAnimation(ACTION::WALK);
		}
		else if (currAction != IDLE && lastPos == sprite.Position)
			ChangeAnimation(ACTION::IDLE);

		//check direction sprite should face
		if (currAction == ACTION::WALK) 
		{
			if ((sprite.Position.x > lastPos.x) && facingLeft) //if creature has moved to the right
			{
				ChangeDirection();
			}
			else if ((sprite.Position.x < lastPos.x) && !facingLeft) //if creature has moved to the left
			{
				ChangeDirection();
			}
		}

		sprite.Update(dTime);
		lastPos = sprite.Position;
		healthBar.Position = Vector2(sprite.Position.x, sprite.Position.y + 100);

		//attack
		if (fightMode) {
			attackTimer += dTime;
			readyToAttack = (attackTimer >= attackCooldown); //returns true if the cooldown is ready
		}
		//damage flash
		if (flashing) 
		{
			flashTimer += dTime;
			if (flashTimer >= damageFlashDuration) {
				sprite.colour = Colours::White;
				flashing = false;
			}
		}

		if (health <= 0) {
			active = false;
			sprite.active = false;
			readyToAttack = false;
			return false;
		}
		return true;
	}
	return true;
}


void Creature::Attack(Creature& target)
{
	target.TakeDamage(attackDmg);
	attackTimer = 0.f;
}

void Creature::TakeDamage(float damage) 
{
	health -= damage;
	flashing = true;
	sprite.colour = Colours::Red;
	flashTimer = 0.f;
	UpdateHealthBar();
}

void Creature::Render(SpriteBatch* Batch,bool renderHealth)
{
	if (active) {
		sprite.Render(Batch);
		if (renderHealth) {
			healthBar.Render(Batch);
		}
	}
}

const char* Creature::getName() 
{
	switch (type) 
	{
	case BUIZEL:
		return "Buizel";
		break;
	case BRELOOM:
		return "Breloom";
		break;
	case SKITTY:
		return "Skitty";
		break;
	case NONE:
		return "Invalid";
		break;
	}
}

const char* Creature::getDescriptor()
{
	switch (type)
	{
	case BUIZEL:
		return "All-Rounder";
		break;
	case BRELOOM:
		return "Tanky Bruiser";
		break;
	case SKITTY:
		return "Ranged Attacker";
		break;
	case NONE:
		return "Invalid";
		break;
	}
}

// make sure to c_str() this! 
int Creature::getValue()
{
	return (10 * upgradeLevel);
}