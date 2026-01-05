#include"TitleScene.h"

#include<DxLib.h>

#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Camera/Camera.h"

#include"../../Application/Application.h"
#include"../SceneManager/SceneManager.h"
#include"../../Utility/Utility.h"

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

	Camera::GetIns().ChangeModeFixedPoint(Vector3(), Vector3());

	skyDome_ = new SkyDome();
	skyDome_->Load();
	skyDome_->SetPos({0.0f,0.0f,0.0f});

	KEY::GetIns().SetMouceFixed(false);
}
void TitleScene::Init(void)
{
	blinkingCounter = 100;
}
void TitleScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		SceneManager::GetIns().PushScene(std::make_shared<EndScene>());
		return;
	}

	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::GAME);
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
	if (skyDome_) {
		skyDome_->Release();
		delete skyDome_;
		skyDome_ = nullptr;
	}
	DeleteGraph(pushToImg);
	DeleteGraph(img_);
}