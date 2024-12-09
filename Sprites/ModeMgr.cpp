#include "ModeMgr.h"
#include "sprite.h"
#include "game.h"



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

void ModeMgr::Render(float dTime, DirectX::SpriteBatch& batch) {
	if (mCurrentMIdx >= 0 && mCurrentMIdx < (int)mModes.size())
		mModes[mCurrentMIdx]->Render(dTime, batch);
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
	//for (size_t i = 0; i < mModes.size(); ++i)
	//	delete mModes[i];
	mModes.clear();
}

MenuMode::MenuMode() 
{
	Sprite _bg("Background", "map4.dds", Game::Get().GetD3D());
	_bg.Init(Vector2(0, 0), Vector2(2, 2), Vector2(0, 0), RECT{ 0,0,512,384 });
	bgSprite = _bg;



	//initialise UI
	UISprite Button("playButton", "playButton.dds", Game::Get().GetD3D());
	Button.Init(Vector2(400, 200), Vector2(1,1), Vector2(0, 0), RECT{ 0,0,144,72 });
	Button.uiType = UISprite::UITYPE::start;
	uiSprites.push_back(Button);

	UISprite Button2("optionsButton", "optionButton.dds", Game::Get().GetD3D());
	Button2.Init(Vector2(400, 300), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,144,72 });
	Button2.uiType = UISprite::UITYPE::options;
	uiSprites.push_back(Button2);


	UISprite Button3("homeButton", "homeButton.dds", Game::Get().GetD3D());
	Button3.Init(Vector2(400, 400), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,144,72 });
	Button3.uiType = UISprite::UITYPE::quit;
	uiSprites.push_back(Button3);

	Sprite Logo("Logo", "Logo.dds", Game::Get().GetD3D());
	Logo.Init(Vector2(230, 75), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,502,89 });
	logoSprite = Logo;

}

void MenuMode::Update(float dTime)
{
	Mouse& mouse = Game::Get().mouse;
	//UPDATE UI
	for (int i = 0; i < uiSprites.size(); i++)
	{
		uiSprites[i].Update(dTime);
		if (isSpriteClickReleased(uiSprites[i], mouse))
		{
			if (uiSprites[i].Action()) {
				UIAction(uiSprites[i]);
			}
		}
	}
}

void MenuMode::Render(float dTime, SpriteBatch& batch) 
{
	bgSprite.Render(&batch);
	logoSprite.Render(&batch);
	for (int i = 0; i < uiSprites.size(); i++) {
		uiSprites[i].Render(&batch);
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
		PostQuitMessage(0);
	}
}