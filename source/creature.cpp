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
		Sprite _sprite("chunkyIdle", "chunkyIdle.dds", Game::Get().GetD3D());
		sprite = _sprite;
		SpriteInit(grid, gridPos, Vector2(2, 2), false, RECT{ 0,0,64,64 }, RECT{ 0,0,64,64 }, 2, 0.8f, isShop);
		//set stats
		maxHealth = 125;
		attackCooldown = 1.15f;
		//set other animations
		Sprite wSprite("chunkyWalk", "chunkyWalk.dds", Game::Get().GetD3D());
		walkSprite = wSprite;
		walkSprite.Init(gridPos, Vector2(2, 2), true, RECT{ 0,0,64,64 }, RECT{ 0,0,64,64 }, 4, 0.3f);
	}
	if (typeToMake == creatureType::BUIZEL)
	{
		Sprite _sprite("Buizel", "mouseIdle.dds", Game::Get().GetD3D());
		sprite = _sprite;
		SpriteInit(grid, gridPos, Vector2(2, 2), false, RECT{ 0,0,64,64 }, RECT{ 0,0,64,64 }, 2, 0.6f, isShop);
		//set other animations
		Sprite wSprite("buizelWalk", "mouseIdle.dds", Game::Get().GetD3D());
		walkSprite = wSprite;
		walkSprite.Init(gridPos, Vector2(2, 2), true, RECT{ 0,0,64,64 }, RECT{ 0,0,64,64 }, 2, 0.3f);
	}
	if (typeToMake == creatureType::SKITTY) 
	{
		Sprite _sprite("Skitty", "mothfoxIdle.dds", Game::Get().GetD3D());
		sprite = _sprite;
		SpriteInit(grid, gridPos, Vector2(2, 2), false, RECT{ 0,0,64,64 }, RECT{ 0,0,64,64 }, 2, 0.7f, isShop);
		//set stats
		attackRange = 300.f;
		maxHealth = 75;
		//set other animations
		Sprite wSprite("skittyWalk", "mothfoxWalk.dds", Game::Get().GetD3D());
		walkSprite = wSprite;
		walkSprite.Init(gridPos, Vector2(2, 2), true, RECT{ 0,0,64,64 }, RECT{ 0,0,64,64 }, 4, 0.3f);
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
	RECT _rect = idleSprite.getSpriteRect();
	RECT _rect2 = walkSprite.getSpriteRect();
	if (!facingLeft) 
	{
		idleSprite.setSpriteRect(RECT{ _rect.left,0,_rect.right, idleSprite.getFrameSize().bottom });
		walkSprite.setSpriteRect(RECT{ _rect2.left,0,_rect2.right, walkSprite.getFrameSize().bottom });
		facingLeft = true;
	}
	else 
	{
		idleSprite.setSpriteRect(RECT{ _rect.left,idleSprite.getFrameSize().bottom,_rect.right, idleSprite.getFrameSize().bottom * 2 });
		walkSprite.setSpriteRect(RECT{ _rect2.left,walkSprite.getFrameSize().bottom,_rect2.right, walkSprite.getFrameSize().bottom * 2 });
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
		healthBar.Position = Vector2(sprite.Position.x+15, sprite.Position.y + 135);

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
		sprite.colour = Colours::White;
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