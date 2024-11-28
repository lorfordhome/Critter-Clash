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
	Sprite idleSprite;
	Sprite walkSprite;
	Sprite attackSprite;
	Vector2 lastPos = { 0,0 };
public:
	creatureType type=creatureType::NONE;
	Sprite sprite; //active sprite
	enum ACTION{IDLE=0,WALK=1,ATTACK=2,DEAD=3};
	ACTION currAction = ACTION::IDLE;
	int Health = 100;
	float attackRange = 76;
	float speed = 0.03f;
	bool active = false;
	bool isEnemy = false;
	int targetIndex=0 ; //index of creature it is targeting within the gameCreatures array
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
	void Update(float dTime);
};