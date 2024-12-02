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
	Vector2 lastPos = { 0,0 };
public:
	creatureType type=creatureType::NONE;
	Sprite sprite; //active sprite
	Sprite idleSprite;
	enum ACTION{IDLE=0,WALK=1,ATTACK=2,DEAD=3};
	ACTION currAction = ACTION::IDLE;
	bool active = true;

	//combat stats
	float attackDmg = 20;
	float attackRange = 76;
	float attackCooldown = 1;
	float attackTimer = 0.f;
	bool readyToAttack = true;
	int targetIndex = 0; //index of creature it is targeting within the gameCreatures array
	float health = 100;
	float maxHealth = 100;

	float speed = 0.03f;
	bool isEnemy = false;

	//damage flash effect
	bool flashing = false;
	float damageFlashDuration = 0.25;
	float flashTimer = 0.f;

	//functions
	void SpriteInit( Grid& grid, Vector2 position, Vector2 scale, bool centerOrigin, RECT spriteRect, RECT framerect, int totalframes, float animspeed);
	Creature(string SpriteName, string path, MyD3D& d3d, bool Enemy = false):isEnemy(Enemy) {
		Sprite _sprite(SpriteName, path, d3d);
		sprite = _sprite;
	}
	Creature(creatureType typeToMake, Vector2 gridPos, Grid& grid, bool Enemy = false);
	void ChangeAnimation(ACTION toChangeTo);
	Creature(bool Enemy = false) :isEnemy(Enemy){}
	Sprite getSprite() {
		return sprite;
	}
	bool Update(float dTime, bool fightMode);
	void Attack(Creature& target);
	void TakeDamage(float damage);
	void ResetCreature();
};