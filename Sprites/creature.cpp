#include "creature.h"
#include "sprite.h"
#include "game.h"

void Creature::SpriteInit( Grid& grid, Vector2 position, Vector2 scale, bool centerOrigin, RECT spriterect, RECT framerect, int totalframes, float animspeed) {
	if (isEnemy)
		sprite.setGridPosition(grid, position.x, position.y,false);
	else
		sprite.setGridPosition(grid, position.x, position.y);
	sprite.Scale = scale;
	sprite.spriteRect = spriterect;
	sprite.frameSize = framerect;
	sprite.totalFrames = totalframes;
	sprite.animSpeed = animspeed;
	sprite.isAnim = true;
	sprite.dim = (RECT{ long(sprite.spriteRect.left * sprite.Scale.x), long(sprite.spriteRect.top * sprite.Scale.y), long(sprite.spriteRect.right * sprite.Scale.x), long(sprite.spriteRect.bottom * sprite.Scale.y) });

	lastPos = sprite.Position;
}


Creature::Creature(creatureType typeToMake, Vector2 gridPos, Grid& grid, bool Enemy):isEnemy(Enemy),type(typeToMake)
{
	if (typeToMake == creatureType::BRELOOM) 
	{
		Sprite _sprite("Breloom", "breloomIdle.dds", Game::Get().GetD3D());
		sprite = _sprite;
		SpriteInit(grid, gridPos, Vector2(3, 3), false, RECT{ 0,96,40,144 }, RECT{ 0,0,40,48 }, 12, 0.1f);
		//set stats
		maxHealth = 125;
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
		attackRange = 300.f;
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

bool Creature::Update(float dTime, bool fightMode) 
{
	if (active) {
		if (lastPos != sprite.Position && currAction != ACTION::WALK)
			ChangeAnimation(ACTION::WALK);
		else if (currAction != IDLE && lastPos == sprite.Position)
			ChangeAnimation(ACTION::IDLE);
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