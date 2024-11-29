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
	for (size_t i = 0; i < mModes.size(); ++i)
		delete mModes[i];
	mModes.clear();
}

MenuMode::MenuMode() 
{
	Sprite _bg("Background", "map4.dds", Game::Get().GetD3D());
	_bg.Init(Vector2(0, 0), Vector2(2, 2), Vector2(0, 0), RECT{ 0,0,512,384 });
	bgSprite = _bg;

	Sprite Button("Button", "startButton.dds", Game::Get().GetD3D());
	Button.Init(Vector2(410, 600), Vector2(0.15, 0.15), Vector2(0, 0), RECT{ 0,0,1280,427 });
	Button.type = Sprite::spriteTYPE::UI;
	uiSprites.push_back(Button);

	Sprite Logo("Logo", "muddyLogo.dds", Game::Get().GetD3D());
	Logo.Init(Vector2(125, 0), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,757,113 });
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
			Game::Get().GetModeMgr().SwitchMode(GAMEMODE::PLAY);
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