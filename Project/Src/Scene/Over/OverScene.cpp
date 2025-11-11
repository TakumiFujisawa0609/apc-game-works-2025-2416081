#include"OverScene.h"

#include"../../Utility/Utility.h"

#include"../../Manager/Input/KeyManager.h"

#include"../../Application/Application.h"
#include"../SceneManager/SceneManager.h"

OverScene::OverScene()
{
}

OverScene::~OverScene()
{
}

void OverScene::Load(void)
{
	KEY::GetIns().SetMouceFixed(false);
	Utility::LoadImg(img_, "Data/Image/Over/GameOver.png");
}

void OverScene::Init(void)
{
}

void OverScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down ||
		KEY::GetIns().GetInfo(KEY_TYPE::GAME_END).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::TITLE);
		return;
	}
}

void OverScene::Draw(void)
{
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

void OverScene::Release(void)
{
	DeleteGraph(img_);
}