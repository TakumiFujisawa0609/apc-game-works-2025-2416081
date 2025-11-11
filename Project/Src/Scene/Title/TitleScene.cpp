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
	camera_(nullptr),
	skyDome_(nullptr)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Load(void)
{
	Utility::LoadImg(img_, "Data/Image/Title/Title.png");

	camera_ = new Camera();
	camera_->Init();
	camera_->ChangeMode(Camera::MODE::FIXED_POINT);

	skyDome_ = new SkyDome();
	skyDome_->Load();
	skyDome_->SetPos({0.0f,0.0f,0.0f});

	KEY::GetIns().SetMouceFixed(false);
}
void TitleScene::Init(void)
{
}
void TitleScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::GAME_END).down) {
		SceneManager::GetIns().PushScene(std::make_shared<EndScene>());
		return;
	}

	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::GAME);
		return;
	}
	skyDome_->Update();
}
void TitleScene::Draw(void)
{
	camera_->Apply();

	skyDome_->Draw();

	DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, img_, true);

	SetFontSize(32);
	if (KEY::GetIns().GetControllerConnect()) {
		DrawString(10, 0, 
			"ゲームスタート：B\n\nゲーム終了：START",
			0xffffff);
	} else {
		DrawString(10, 0,
			"ゲームスタート：SPACE\n\nゲーム終了：ESC",
			0xffffff);
	}
	SetFontSize(16);
}
void TitleScene::Release(void)
{
	if (skyDome_) {
		skyDome_->Release();
		delete skyDome_;
		skyDome_ = nullptr;
	}
	if (camera_) {
		camera_->Release();
		delete camera_;
		camera_ = nullptr;
	}
	DeleteGraph(img_);
}