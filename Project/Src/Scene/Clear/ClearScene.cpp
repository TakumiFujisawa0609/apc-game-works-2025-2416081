#include "ClearScene.h"

#include"../../Utility/Utility.h"

#include"../../Manager/Input/KeyManager.h"

#include"../../Application/Application.h"
#include"../SceneManager/SceneManager.h"

#include"../../Object/SkyDome/SkyDome.h"

ClearScene::ClearScene() :
	img_(-1),
	skyDome_(nullptr)
{
}

ClearScene::~ClearScene()
{
}

void ClearScene::Load(void)
{
	KEY::GetIns().SetMouceFixed(false);
	Utility::LoadImg(img_, "Data/Image/Clear/GameClear.png");

	skyDome_ = new SkyDome();
	skyDome_->Load();
}

void ClearScene::Init(void)
{
}

void ClearScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down ||
		KEY::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::TITLE);
		return;
	}
	skyDome_->Update();
}

void ClearScene::Draw(void)
{
	skyDome_->Draw();
	DrawRotaGraph(App::SCREEN_SIZE_X / 2, App::SCREEN_SIZE_Y / 2, 1, 0, img_, true);

	SetFontSize(32);
	if (KEY::GetIns().GetControllerConnect()) {
		DrawString(10, 0,
			"タイトルへ：B",
			0xffffff);
	}
	else {
		DrawString(10, 0,
			"タイトルへ：SPACE",
			0xffffff);
	}
	SetFontSize(16);
}

void ClearScene::Release(void)
{
	if (skyDome_) {
		skyDome_->Release();
		delete skyDome_;
		skyDome_ = nullptr;
	}
	DeleteGraph(img_);
}