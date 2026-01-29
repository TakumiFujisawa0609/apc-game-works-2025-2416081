#include"TitleScene.h"

#include<DxLib.h>
#include"../../Utility/Utility.h"

#include"../../Application/Application.h"

#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Camera/Camera.h"
#include"../../Manager/Sound/SoundManager.h"

#include"../SceneManager/SceneManager.h"

#include"End/EndScene.h"

#include"../../Object/SkyDome/SkyDome.h"


TitleScene::TitleScene():
	img_(-1),
	pushToImg(-1),

	blinkingCounter(0),
	blinkingSigned(5),

	skyDome_(nullptr)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Load(void)
{
	Utility::LoadImg(img_, "Data/Image/Title/Title.png");
	Utility::LoadImg(pushToImg, "Data/Image/Title/PushToStart.png");

	skyDome_ = new SkyDome();
	skyDome_->Load();
	skyDome_->SetPos({0.0f,0.0f,0.0f});

	KEY::GetIns().SetMouceFixed(false);

	Smng::GetIns().Load(SOUND::BGM_TITLE);
}
void TitleScene::Init(void)
{
	Camera::GetIns().ChangeModeFixedPoint(Vector3(), Vector3());

	blinkingCounter = 100;

	Smng::GetIns().Play(SOUND::BGM_BATTLE, false, 80, true);
}
void TitleScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		SceneManager::GetIns().PushScene(std::make_shared<EndScene>());
		Smng::GetIns().Play(SOUND::SE_SYSTEM_SELECT, true, 100);
		return;
	}

	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::RANKING);
		Smng::GetIns().Play(SOUND::SE_SYSTEM_BUTTON, true, 150);
		return;
	}
	skyDome_->Update();

	blinkingCounter += blinkingSigned;
	if (blinkingCounter <= 50 || blinkingCounter > 240) { blinkingSigned *= -1; }
}
void TitleScene::Draw(void)
{
	skyDome_->Draw();

	DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, img_, true);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, blinkingCounter);
	DrawRotaGraph(App::SCREEN_SIZE_X / 2, (int)(App::SCREEN_SIZE_Y * 0.92f), 1, 0, pushToImg, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//SetFontSize(32);
	//if (KEY::GetIns().GetControllerConnect()) {
	//	DrawString(10, 0, 
	//		"ゲームスタート：B\n\nゲーム終了：START",
	//		0xffffff);
	//} else {
	//	DrawString(10, 0,
	//		"ゲームスタート：SPACE\n\nゲーム終了：ESC",
	//		0xffffff);
	//}
	//SetFontSize(16);
}
void TitleScene::Release(void)
{
	Smng::GetIns().Delete(SOUND::BGM_TITLE);
	if (skyDome_) {
		skyDome_->Release();
		delete skyDome_;
		skyDome_ = nullptr;
	}
	DeleteGraph(pushToImg);
	DeleteGraph(img_);
}