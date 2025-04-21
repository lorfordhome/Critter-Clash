#include "creature.h"
#include "sprite.h"
#include "game.h"

void Creature::SpriteInit( Grid& grid, raylib::Vector2 position, raylib::Vector2 scale, bool centerOrigin, raylib::Rectangle spriterect, raylib::Rectangle framerect, int totalframes, float animspeed, bool isShop) {
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
	sprite.dim = (raylib::Rectangle{ (sprite.spriteRect.x * sprite.Scale.x), (sprite.spriteRect.y * sprite.Scale.y), (sprite.spriteRect.width * sprite.Scale.x), (sprite.spriteRect.height * sprite.Scale.y) });

	lastPos = sprite.Position;
}


Creature::Creature(creatureType typeToMake, raylib::Vector2 gridPos, Grid& grid, bool isShop, bool Enemy):isEnemy(Enemy),type(typeToMake)
{
	if (typeToMake == creatureType::BRELOOM) 
	{
		Sprite _sprite("chunkyIdle", "chunkyIdle.dds");
		sprite = _sprite;
		SpriteInit(grid, gridPos, raylib::Vector2(2, 2), false, raylib::Rectangle{ 0,0,64,64 }, raylib::Rectangle{ 0,0,64,64 }, 2, 0.8f, isShop);
		//set stats
		maxHealth = 125;
		attackCooldown = 1.15f;
		//set other animations
		Sprite wSprite("chunkyWalk", "chunkyWalk.dds");
		walkSprite = wSprite;
		walkSprite.Init(gridPos, raylib::Vector2(2, 2), true, raylib::Rectangle{ 0,0,64,64 }, raylib::Rectangle{ 0,0,64,64 }, 4, 0.3f);
	}
	if (typeToMake == creatureType::BUIZEL)
	{
		Sprite _sprite("Buizel", "mouseIdle.dds");
		sprite = _sprite;
		SpriteInit(grid, gridPos, raylib::Vector2(2, 2), false, raylib::Rectangle{ 0,0,64,64 }, raylib::Rectangle{ 0,0,64,64 }, 2, 0.6f, isShop);
		//set other animations
		Sprite wSprite("buizelWalk", "mouseIdle.dds");
		walkSprite = wSprite;
		walkSprite.Init(gridPos, raylib::Vector2(2, 2), true, raylib::Rectangle{ 0,0,64,64 }, raylib::Rectangle{ 0,0,64,64 }, 2, 0.3f);
	}
	if (typeToMake == creatureType::SKITTY) 
	{
		Sprite _sprite("Skitty", "mothfoxIdle.dds");
		sprite = _sprite;
		SpriteInit(grid, gridPos, raylib::Vector2(2, 2), false, raylib::Rectangle{ 0,0,64,64 }, raylib::Rectangle{ 0,0,64,64 }, 2, 0.7f, isShop);
		//set stats
		attackRange = 300.f;
		maxHealth = 75;
		//set other animations
		Sprite wSprite("skittyWalk", "mothfoxWalk.dds");
		walkSprite = wSprite;
		walkSprite.Init(gridPos, raylib::Vector2(2, 2), true, raylib::Rectangle{ 0,0,64,64 }, raylib::Rectangle{ 0,0,64,64 }, 4, 0.3f);
	}
	health = maxHealth;
	idleSprite = sprite;

	//initialise healthbar
	if (isEnemy) {
		Sprite hSprite("enemyHealth", "Enemy Health Bar.dds");
		healthBar = hSprite;
		healthBar.Init(raylib::Vector2(0, 0), raylib::Vector2(0.35, 0.5), raylib::Vector2(0, 0), 
			
			{ 0,0,277,11 });
		ChangeDirection();
	}
	else {
		Sprite hSprite("playerHealth", "Player Health Bar.dds");
		healthBar = hSprite;
		healthBar.Init(raylib::Vector2(0, 0), raylib::Vector2(0.35, 0.5), raylib::Vector2(0, 0), raylib::Rectangle{ 0,0,277,11 });

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
	if (!facingLeft && !isEnemy)
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
	raylib::Rectangle _rect = idleSprite.getSpriteRect();
	raylib::Rectangle _rect2 = walkSprite.getSpriteRect();
	if (!facingLeft) 
	{
		//make them face left (bottom row of sprites)
		idleSprite.setSpriteRect(raylib::Rectangle{ _rect.x,0,_rect.width, idleSprite.getFrameSize().height });
		walkSprite.setSpriteRect(raylib::Rectangle{ _rect2.x,0,_rect2.width, walkSprite.getFrameSize().height });
		facingLeft = true;
	}
	else 
	{
		//make them face right (top row of sprites)
		idleSprite.setSpriteRect(raylib::Rectangle{ _rect.x,idleSprite.getFrameSize().height,_rect.width, idleSprite.getFrameSize().height });
		walkSprite.setSpriteRect(raylib::Rectangle{ _rect2.x,walkSprite.getFrameSize().height,_rect2.width, walkSprite.getFrameSize().height});
		facingLeft = false;
	}
	ChangeAnimation(currAction);
}

bool Creature::Update(float dTime, bool fightMode, bool isShop) //update returns false if the creature has died
{
	if (isShop) 
	{
		sprite.Update(dTime);
		//damage flash
		if (flashing)
		{
			DamageFlash(dTime);
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
			if ((sprite.Position.x > lastPos.x) && !facingLeft) //if creature has moved to the right
			{
				ChangeDirection();
			}
			else if ((sprite.Position.x < lastPos.x) && facingLeft) //if creature has moved to the left
			{
				ChangeDirection();
			}
		}
		else if (isEnemy && facingLeft)
			ChangeDirection();

		sprite.Update(dTime);
		lastPos = sprite.Position;
		healthBar.Position = raylib::Vector2(sprite.Position.x+15, sprite.Position.y + 135);

		//attack
		if (fightMode) {
			attackTimer += dTime;
			readyToAttack = (attackTimer >= attackCooldown); //returns true if the cooldown is ready
		}
		//damage flash
		if (flashing)
		{
			DamageFlash(dTime);
		}

		if (health <= 0) {
			PlayDeathSFX();
			active = false;
			sprite.active = false;
			readyToAttack = false;
			return false;
		}
		return true;
	}
	return true;
}


void Creature::PlayDeathSFX() 
{
	if (isEnemy) {
		Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("EnemyDeath"), false, false, &Game::Get().sfxHdl);
	}
	else 
	{
		switch (type) {
		case BUIZEL:
			Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("BuizelSad"), false, false, &Game::Get().sfxHdl);
			break;
		case BRELOOM:
			Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("BreloomSad"), false, false, &Game::Get().sfxHdl);
			break;
		case SKITTY:
			Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("SkittySad"), false, false, &Game::Get().sfxHdl);
			break;
		}
	}
}
void Creature::DamageFlash(float dTime) {
	flashTimer += dTime;
	if (flashTimer >= damageFlashDuration) {
		sprite.colour = raylib::Color::White();
		flashing = false;
	}
}

void Creature::Attack(Creature& target)
{
	target.TakeDamage(attackDmg);
	attackTimer = 0.f;
	//play sfx
	int soundIdx = std::rand() % 3;
	if (soundIdx == 0) {
		Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("strike1"), false, false, &Game::Get().sfxHdl);
	}
	if (soundIdx == 1) {
		Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("strike2"), false, false, &Game::Get().sfxHdl);
	}
	if (soundIdx == 2) {
		Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("strike3"), false, false, &Game::Get().sfxHdl);
	}
}

void Creature::TakeDamage(float damage) 
{
	health -= damage;
	flashing = true;
	sprite.colour = raylib::Color::Red();
	flashTimer = 0.f;
	UpdateHealthBar();
}

void Creature::Render(bool renderHealth)
{
	if (active) {
		sprite.Render();
		if (renderHealth) {
			healthBar.Render();
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
		break;
	}
	return "Invalid";
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
		break;
	}
	return "Invalid";
}

// make sure to c_str() this! 
int Creature::getValue()
{
	return (10 * upgradeLevel);
}