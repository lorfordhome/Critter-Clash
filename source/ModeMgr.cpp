#include "ModeMgr.h"
#include "sprite.h"
#include "game.h"

GAMEMODE ModeMgr::GetModeName() {
	return (mModes[mCurrentMIdx]->GetMName());
}
AMode* ModeMgr::GetMode() {
	return (mModes[mCurrentMIdx]);
}

void ModeMgr::SwitchMode(GAMEMODE newMode) {
	int idx = 0;
	assert(!mModes.empty());
	while (idx < (int)mModes.size() && mModes[idx]->GetMName() != newMode)
		++idx;
	assert(idx < (int)mModes.size());
	mDesiredMIdx = idx;
}

void ModeMgr::Update(float dTime) {
	if (mDesiredMIdx != mCurrentMIdx)
		if (mCurrentMIdx == -1 || mModes[mCurrentMIdx]->Exit())
		{
			mCurrentMIdx = mDesiredMIdx;
			mModes[mCurrentMIdx]->Enter();
		}
	mModes[mCurrentMIdx]->Update(dTime);
}

void ModeMgr::Render(float dTime) {
	if (mCurrentMIdx >= 0 && mCurrentMIdx < (int)mModes.size())
		mModes[mCurrentMIdx]->Render(dTime);
}

void ModeMgr::ProcessKey(char key) {
	if (mCurrentMIdx >= 0 && mCurrentMIdx < (int)mModes.size())
		mModes[mCurrentMIdx]->ProcessKey(key);
}

void ModeMgr::AddMode(AMode* p) {
	assert(p);
	mModes.push_back(p);
}

void ModeMgr::Release() {
	for (size_t i = 0; i < mModes.size(); ++i)
		delete mModes[i];
	mModes.clear();
}

void ModeMgr::DeleteMode(GAMEMODE modeToDelete) {
	int idx = 0;
	assert(!mModes.empty());
	while (idx < (int)mModes.size() && mModes[idx]->GetMName() != modeToDelete)
		++idx;
	assert(idx < (int)mModes.size());
	delete mModes[idx];
	auto it = mModes.begin();
	it += idx;
	mModes.erase(it);

}


MenuMode::MenuMode() 
{
	Sprite _bg("Background", "map4.dds");
	_bg.Init(raylib::Vector2(0, 0), raylib::Vector2(2, 2), raylib::Vector2(0, 0), raylib::Rectangle{ 0,0,512,384 });
	bgSprite = _bg;



	//initialise UI
	UISprite Button("playButton", "playButton.png");
	Button.Init(raylib::Vector2(400, 200), raylib::Vector2(1,1), raylib::Vector2(0, 0), raylib::Rectangle{ 0,0,144,72 });
	Button.uiType = UISprite::UITYPE::start;
	uiSprites.push_back(Button);

	UISprite Button3("homeButton", "homeButton.dds");
	Button3.Init(raylib::Vector2(400, 300), raylib::Vector2(1, 1), raylib::Vector2(0, 0), raylib::Rectangle{ 0,0,144,72 });
	Button3.uiType = UISprite::UITYPE::quit;
	uiSprites.push_back(Button3);

	Sprite Logo("Logo", "Logo.dds");
	Logo.Init(raylib::Vector2(230, 75), raylib::Vector2(1, 1), raylib::Vector2(0, 0), raylib::Rectangle{ 0,0,502,89 });
	logoSprite = Logo;


}

void MenuMode::Update(float dTime)
{
	if (!Game::Get().getAudioMgr().GetSongMgr()->IsPlaying(Game::Get().musicHdl)) {
		Game::Get().getAudioMgr().GetSongMgr()->Play(utf8string("MenuMusic"), true, false, &Game::Get().musicHdl, Game::Get().getAudioMgr().GetSongMgr()->GetVolume());
	}
	//UPDATE UI
	for (size_t i = 0; i < uiSprites.size(); i++)
	{
		uiSprites[i].Update(dTime);
		if (isSpriteClickReleased(uiSprites[i]))
		{
			if (uiSprites[i].Action()) {
				UIAction(uiSprites[i]);
			}
		}
	}
}

void MenuMode::Render(float dTime) 
{

	logoSprite.Render();
	for (size_t i = 0; i < uiSprites.size(); i++) {
		uiSprites[i].Render();
	}

}

void MenuMode::UIAction(UISprite& sprite)
{
	if (sprite.uiType == UISprite::UITYPE::start)
	{
		Game::Get().GetModeMgr().SwitchMode(GAMEMODE::PLAY);
	}
	else if (sprite.uiType == UISprite::UITYPE::options)
	{

	}
	else if (sprite.uiType == UISprite::UITYPE::quit)
	{
		Game::Get().GetRango().windowShouldClose = true;
	}
}