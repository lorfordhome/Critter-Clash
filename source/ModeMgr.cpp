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

void Setup(Model& m, Mesh& source, const Vector3& scale, const Vector3& pos, const Vector3& rot)
{
	m.Initialise(source);
	m.GetScale() = scale;
	m.GetPosition() = pos;
	m.GetRotation() = rot;
}

void Setup(Model& m, Mesh& source, float scale, const Vector3& pos, const Vector3& rot)
{
	Setup(m, source, Vector3(scale, scale, scale), pos, rot);
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

	UISprite Button3("homeButton", "homeButton.dds", Game::Get().GetD3D());
	Button3.Init(Vector2(400, 300), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,144,72 });
	Button3.uiType = UISprite::UITYPE::quit;
	uiSprites.push_back(Button3);

	Sprite Logo("Logo", "Logo.dds", Game::Get().GetD3D());
	Logo.Init(Vector2(230, 75), Vector2(1, 1), Vector2(0, 0), RECT{ 0,0,502,89 });
	logoSprite = Logo;

	//init 3d
	Model m;
	mModels.insert(mModels.begin(), 2, m);

	Mesh& quadMesh = BuildQuad(Game::Get().GetD3D().GetMeshMgr());
	Mesh& cubeMesh = BuildCube(Game::Get().GetD3D().GetMeshMgr());

	Mesh& br = Game::Get().GetD3D().GetMeshMgr().CreateMesh("Breloom");
	br.CreateFrom("../bin/data/Breloom/model.obj", Game::Get().GetD3D());
	Setup(mModels[0], br, .5f, Vector3(1.2, 0.5f, -2.5f), Vector3(PI*2, 0, 0));
	Game::Get().GetD3D().GetFX().SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.47f, 0.47f, 0.47f), Vector3(0.15f, 0.15f, 0.15f), Vector3(0.25f, 0.25f, 0.25f));

	Mesh& sk = Game::Get().GetD3D().GetMeshMgr().CreateMesh("Skitty");
	sk.CreateFrom("../bin/data/Skitty/model.obj", Game::Get().GetD3D());
	Setup(mModels[1], sk, .5f, Vector3(-0.4, -0.5f, -2.5f), Vector3(PI * 2, 0, 0));
	Game::Get().GetD3D().GetFX().SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.47f, 0.47f, 0.47f), Vector3(0.15f, 0.15f, 0.15f), Vector3(0.25f, 0.25f, 0.25f));

}

void MenuMode::Update(float dTime)
{
	if (!Game::Get().getAudioMgr().GetSongMgr()->IsPlaying(Game::Get().musicHdl)) {
		Game::Get().getAudioMgr().GetSongMgr()->Play(utf8string("MenuMusic"), true, false, &Game::Get().musicHdl, Game::Get().getAudioMgr().GetSongMgr()->GetVolume());
	}
	Mouse& mouse = Game::Get().mouse;
	//UPDATE UI
	for (size_t i = 0; i < uiSprites.size(); i++)
	{
		uiSprites[i].Update(dTime);
		if (isSpriteClickReleased(uiSprites[i], mouse))
		{
			if (uiSprites[i].Action()) {
				UIAction(uiSprites[i]);
			}
		}
	}

	//Update 3D models
	for (size_t i = 0; i < mModels.size(); ++i) {
		mModels[i].GetPosition().y = (sinf(2 * GetClock() + (PI / 4) * i)) * 0.2f;
		mModels[i].GetRotation().y += (i < 2) ? dTime : -dTime;
	}
}

void MenuMode::Render(float dTime, SpriteBatch& batch) 
{

	logoSprite.Render(&batch);
	for (size_t i = 0; i < uiSprites.size(); i++) {
		uiSprites[i].Render(&batch);
	}


	//3d rendering begin
	mCamPos = Vector3(1, 1, -5);
	float alpha = 0.5f + sinf(gAngle * 2) * 0.5f;

	Game::Get().GetD3D().GetFX().SetPerFrameConsts(Game::Get().GetD3D().GetDeviceCtx(), mCamPos);

	CreateViewMatrix(Game::Get().GetD3D().GetFX().GetViewMatrix(), mCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
	CreateProjectionMatrix(Game::Get().GetD3D().GetFX().GetProjectionMatrix(), 0.25f * PI, WinUtil::Get().GetAspectRatio(), 1, 1000.f);
	Matrix w = Matrix::CreateRotationY(sinf(gAngle));
	Game::Get().GetD3D().GetFX().SetPerObjConsts(Game::Get().GetD3D().GetDeviceCtx(), w);

	Vector3 dir = Vector3(1, 0, 0);
	Matrix m = Matrix::CreateRotationY(gAngle);
	dir = dir.TransformNormal(dir, m);
	dir *= -1;
	float d = sinf(gAngle) * 0.5f + 0.5f;


	for (auto& mod : mModels)
		Game::Get().GetD3D().GetFX().Render(mod);
	//3d rendering end
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