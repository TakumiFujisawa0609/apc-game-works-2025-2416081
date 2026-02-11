#include"OverScene.h"

#include"../../Utility/Utility.h"

#include"../../Application/Application.h"

#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Sound/SoundManager.h"
#include"../../Manager/Camera/Camera.h"

#include"../SceneManager/SceneManager.h"

OverScene::OverScene() :
	img_(-1)
{
}

OverScene::~OverScene()
{
}

void OverScene::Load(void)
{
	Snd::GetIns().ChangeScene("Over");
	KEY::GetIns().SetMouceFixed(false);
	LoadImg(img_, "Data/Image/Over/GameOver.png");
}

void OverScene::Init(void)
{
	Camera::GetIns().ChangeModeFixedPoint(Vector3(), Vector3());

	Snd::GetIns().Play("OverBgm");
}

void OverScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down ||
		KEY::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::TITLE);
		Snd::GetIns().Play("SystemButton");
		return;
	}
}

void OverScene::Draw(void)
{
	DrawRotaGraph(App::SCREEN_SIZE_X / 2, App::SCREEN_SIZE_Y / 2, 1, 0, img_, true);
}

void OverScene::Release(void)
{
	DeleteGraph(img_);
}