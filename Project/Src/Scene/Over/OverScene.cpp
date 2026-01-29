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
	KEY::GetIns().SetMouceFixed(false);
	Utility::LoadImg(img_, "Data/Image/Over/GameOver.png");

	Smng::GetIns().Load(SOUND::GAME_OVER);
}

void OverScene::Init(void)
{
	Camera::GetIns().ChangeModeFixedPoint(Vector3(), Vector3());

	Smng::GetIns().Play(SOUND::GAME_OVER, true, 150);
}

void OverScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down ||
		KEY::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::TITLE);
		Smng::GetIns().Play(SOUND::SE_SYSTEM_BUTTON, true, 150);
		return;
	}
}

void OverScene::Draw(void)
{
	DrawRotaGraph(App::SCREEN_SIZE_X / 2, App::SCREEN_SIZE_Y / 2, 1, 0, img_, true);
}

void OverScene::Release(void)
{
	Smng::GetIns().Delete(SOUND::GAME_OVER);
	DeleteGraph(img_);
}