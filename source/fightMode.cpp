#pragma once
#include <string>
#include <cassert>
#include "sprite.h"
#include "game.h"
#include "LuaHelper.h"

void PlayMode::InitLose()
{
	state = State::LOSE;
	//init text
	Sprite Logo("defeatText", "defeatText.dds");
	Logo.Init(raylib::Vector2(350, 50), raylib::Vector2(1, 1), raylib::Vector2(0, 0), raylib::Rectangle{ 0,0,313,73 });
	logoSprite = Logo;
	//init ui
	UISprite Button3("homeButton", "homeButton.dds");
	Button3.Init(raylib::Vector2(410, 300), raylib::Vector2(1, 1), raylib::Vector2(0, 0), raylib::Rectangle{ 0,0,144,72 });
	Button3.type = Sprite::spriteTYPE::UI;
	Button3.uiType = UISprite::UITYPE::menu;
	uiSprites.push_back(Button3);

	UISprite Button("restartButton", "restartButton.dds");
	Button.Init(raylib::Vector2(410, 400), raylib::Vector2(1, 1), raylib::Vector2(0, 0), raylib::Rectangle{ 0,0,144,72 });
	Button.type = Sprite::spriteTYPE::UI;
	Button.uiType = UISprite::UITYPE::restart;
	uiSprites.push_back(Button);
	if (Game::Get().getAudioMgr().GetSongMgr()->IsPlaying(Game::Get().musicHdl)) {
		Game::Get().getAudioMgr().GetSongMgr()->Stop();
	}
	Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("Defeat"), false, false, &Game::Get().sfxHdl);
}
void PlayMode::InitWin()
{
	state = State::WIN;
	//init text
	Sprite Logo("victoryText", "victoryText.dds");
	Logo.Init(raylib::Vector2(350, 50), raylib::Vector2(1, 1), raylib::Vector2(0, 0), raylib::Rectangle{ 0,0,316,91 });
	logoSprite = Logo;
	//init ui
	UISprite Button("nextButton", "nextButton.dds");
	Button.Init(raylib::Vector2(420, 200), raylib::Vector2(1, 1), raylib::Vector2(0, 0), raylib::Rectangle{ 0,0,144,72 });
	Button.type = Sprite::spriteTYPE::UI;
	Button.uiType = UISprite::UITYPE::next;
	uiSprites.push_back(Button);
	if (Game::Get().getAudioMgr().GetSongMgr()->IsPlaying(Game::Get().musicHdl)) {
		Game::Get().getAudioMgr().GetSongMgr()->Stop();
	}
	Game::Get().getAudioMgr().GetSfxMgr()->Play(utf8string("Victory"), false, false, &Game::Get().sfxHdl);

}

void PlayMode::FightUpdate(float dTime)
{
	for (int i = 0; i < gameCreatures.size(); i++)
	{
		if (gameCreatures[i].active)
		{
			//update returns false if the creature has died
			if (!gameCreatures[i].Update(dTime, true))
			{
				if (gameCreatures[i].isEnemy) {
					enemiesAlive--;
				}
				else
					teamAlive--;
			}
			gameCreatures[i].targetIndex = findClosest(i, gameCreatures[i].isEnemy); //find closest potential target

			if (!checkCol(gameCreatures[i], gameCreatures[gameCreatures[i].targetIndex])) //if the creature isn't already within attack range of it's target
			{
				raylib::Vector2 posToMove = MoveTowards(gameCreatures[i].sprite.Position,
					gameCreatures[gameCreatures[i].targetIndex].sprite.Position,
					gameCreatures[i].speed * dTime);
				gameCreatures[i].sprite.setPos(posToMove);
			}
			else
			{
				if (gameCreatures[i].readyToAttack)
					gameCreatures[i].Attack(gameCreatures[gameCreatures[i].targetIndex]);
			}
		}
	}
	//check for end
	if (enemiesAlive <= 0)
		InitWin();
	if (teamAlive <= 0)
		InitLose();
}

void PlayMode::FightRender(float dTime)
{

	bgSprite.Render();
	for (int i = 0; i < gameCreatures.size(); i++) {
		gameCreatures[i].Render();
	}
	for (int i = 0; i < uiSprites.size(); i++) {
		uiSprites[i].Render();
	}
	if (state == State::WIN || state == State::LOSE) {
		logoSprite.Render();
	}
	if (state == State::LOSE)
	{
		Game::Get().GetRango().GetFont().DrawText("You got to Round " + to_string(currentRound), raylib::Vector2(265, 150), Game::Get().GetRango().GetFontSizeLarge(), 2);

	}
}


