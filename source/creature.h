#pragma once
#include "sprite.h"
enum creatureType
{
	NONE,
	BRELOOM,
	BUIZEL,
	SKITTY
};

class Creature 
{
private:
	Sprite walkSprite;
	Sprite attackSprite;
	Sprite healthBar;
	void ChangeDirection();
	void PlayDeathSFX();
public:
	creatureType type=creatureType::NONE;
	Sprite sprite; //active sprite
	Sprite idleSprite;
	enum ACTION{IDLE=0,WALK=1,ATTACK=2,DEAD=3};
	ACTION currAction = ACTION::IDLE;
	bool active = true;
	bool isEnemy = false;
	bool facingLeft = true; //false if creature is facing right
	int targetIndex = 0; //index of creature it is targeting within the gameCreatures array

	Vector2 lastPos = { 0,0 };
	unsigned char upgradeLevel = 1;
	//combat stats
	float attackDmg = 20;
	float attackRange = 76;
	float attackCooldown = 1;
	float attackTimer = 0.f;
	bool readyToAttack = true;
	float health = 100;
	float maxHealth = 100;
	float speed = 0.03f;

	//damage flash effect
	bool flashing = false;
	float damageFlashDuration = 0.25;
	float flashTimer = 0.f;

	//functions
	void SpriteInit( Grid& grid, Vector2 position, Vector2 scale, bool centerOrigin, RECT spriteRect, RECT framerect, int totalframes, float animspeed, bool isShop=false);
	Creature(string SpriteName, string path, MyD3D& d3d, bool Enemy = false):isEnemy(Enemy) {
		Sprite _sprite(SpriteName, path, d3d);
		sprite = _sprite;
	}
	Creature(creatureType typeToMake, Vector2 gridPos, Grid& grid, bool isShop=false, bool Enemy = false);
	void ChangeAnimation(ACTION toChangeTo);
	void UpdateHealthBar();
	Creature(bool Enemy = false) :isEnemy(Enemy){}
	Sprite getSprite() {
		return sprite;
	}
	bool Update(float dTime, bool fightMode, bool isShop=false);
	void Attack(Creature& target);
	void TakeDamage(float damage);
	void Render(SpriteBatch* Batch,bool renderHealth=true);
	void ResetCreature();


	//functions for generating shop display data
	const char* getName();
	const char* getDescriptor();
	int getValue();
};