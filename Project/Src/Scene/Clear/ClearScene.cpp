#include "ClearScene.h"

#include"../../Utility/Utility.h"

#include"../../Application/Application.h"

#include"../SceneManager/SceneManager.h"
#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Camera/Camera.h"
#include"../../Manager/Score/Score.h"

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
	Camera::GetIns().ChangeModeFixedPoint(Vector3(), Vector3());
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
	int x = Application::SCREEN_SIZE_X;
	int y = Application::SCREEN_SIZE_Y;

	skyDome_->Draw();
	DrawExtendGraph(0, 0, x, y, img_, true);

	SetFontSize(64);

	DrawFormatStringF(x / 2.0f - 300.0f, y / 2.0f + 325.0f, 0xffffff, "SCORE : %08d", Score::GetIns().TotalScore());

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