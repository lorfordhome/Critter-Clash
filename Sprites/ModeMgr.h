#pragma once

#include <vector>
#include <string>

#include "D3D.h"
#include "SpriteBatch.h"
#include "sprite.h"
enum class GAMEMODE { INTRO, MENU, PLAY, OVER };
/*
ABC representing a game mode like intro, game, gameOver, highScores, etc.
*/
//interface
class AMode
{
public:
	virtual ~AMode() {}
	/*
	Called on the old mode when switching to the new one
	The switch won't complete until we return true, gives time for 
	effects like fading out.
	*/
	virtual bool Exit() { return true; }
	//called on the new mode just once when it first activates
	virtual void Enter() {}
	//once active this is called repeatedly, contains the logic of the mode
	virtual void Update(float dTime) = 0;
	//used by a mode to render itself
	virtual void Render(float dTime, DirectX::SpriteBatch& batch) = 0;
	//get a mode's name
	virtual GAMEMODE GetMName() const = 0;
	//pass WM_CHAR key messages to a mode
	virtual void ProcessKey(char key) {};
};

class MenuMode : public AMode 
{
	std::vector<Sprite> uiSprites;
	Sprite bgSprite;
	Sprite logoSprite;
public:
	MenuMode();
	void Update(float dTime) override;
	void Render(float dTime, SpriteBatch& batch) override;
	static const GAMEMODE MODE_NAME = GAMEMODE::MENU;
	GAMEMODE GetMName() const override {
		return GAMEMODE::MENU;
	}


};

/*
organise all the mode instances and call them at the right time, switch between them
*/
class ModeMgr
{
public:
	~ModeMgr() {
		Release();
	}
	//change mode
	void SwitchMode(GAMEMODE newMode);
	//update the current mode
	void Update(float dTime);
	//render the current mode
	void Render(float dTime, DirectX::SpriteBatch& batch);
	//send a key to the current mode
	void ProcessKey(char key);
	//add a new mode, these should be dynamically allocated
	void AddMode(AMode* p);
	//free all the mode instances, can be called explicitly or
	//left to the destructor
	void Release();

private:
	std::vector<AMode*> mModes;	//container of modes
	int mCurrentMIdx = -1;		//the one that is active
	int mDesiredMIdx = -1;		//one that wants to be active
};
