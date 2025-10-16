#include "ClearScene.h"

#include"../../Utility/Utility.h"

#include"../../Manager/Input/KeyManager.h"

#include"../SceneManager/SceneManager.h"

ClearScene::ClearScene() :
	img_(-1)
{
}

ClearScene::~ClearScene()
{
}

void ClearScene::Load(void)
{
	KEY::GetIns().SetMouceFixed(false);
}

void ClearScene::Init(void)
{
}

void ClearScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down ||
		KEY::GetIns().GetInfo(KEY_TYPE::GAME_END).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::TITLE);
	}
}

void ClearScene::Draw(void)
{
	SetFontSize(32);
	if (KEY::GetIns().GetControllerConnect()) {
		DrawString(10, 0,
			"クリア\n\nタイトルへ：B",
			0xffffff);
	}
	else {
		DrawString(10, 0,
			"クリア\n\nタイトルへ：SPACE",
			0xffffff);
	}
	SetFontSize(16);
}

void ClearScene::Release(void)
{
}