#include "creature.h"

void Creature::SpriteInit(std::vector<Sprite>& sprites, Grid& grid, Vector2 position, Vector2 scale, bool centerOrigin, RECT spriterect, RECT framerect, int totalframes, float animspeed) {
	sprite.setGridPosition(grid, position.x, position.y);
	sprite.Scale = scale;
	sprite.spriteRect = spriterect;
	sprite.frameSize = framerect;
	sprite.totalFrames = totalframes;
	sprite.animSpeed = animspeed;
	sprite.isAnim = true;
	sprite.dim = (RECT{ long(sprite.spriteRect.left * sprite.Scale.x), long(sprite.spriteRect.top * sprite.Scale.y), long(sprite.spriteRect.right * sprite.Scale.x), long(sprite.spriteRect.bottom * sprite.Scale.y) });
	sprites.push_back(sprite);
}