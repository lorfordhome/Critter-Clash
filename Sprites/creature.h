#pragma once
#include "sprite.h"
enum creatureType
{
	BRELOOM,
	BUIZEL,
	SKITTY
};

class Creature 
{
public:
	Sprite sprite;
	enum ACTION{IDLE=0,WALK=1,ATTACK=2,DEAD=3};
	ACTION currAction = ACTION::IDLE;
	int Health = 100;
	float attackRange = 76;
	float speed = 0.03f;
	bool active = false;
	bool isEnemy = false;
	int targetIndex=0 ; //index of creature is is targeting within the gameCreatures array
	void SpriteInit( Grid& grid, Vector2 position, Vector2 scale, bool centerOrigin, RECT spriteRect, RECT framerect, int totalframes, float animspeed);
	Creature(string SpriteName, string path, MyD3D& d3d, bool Enemy = false):isEnemy(Enemy) {
		Sprite _sprite(SpriteName, path, d3d);
		sprite = _sprite;
	}
	Creature(bool Enemy = false) :isEnemy(Enemy){}
	Sprite getSprite() {
		return sprite;
	}
};