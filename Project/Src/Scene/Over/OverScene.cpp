#include"OverScene.h"

#include"../../Utility/Utility.h"

#include"../../Manager/Input/KeyManager.h"

#include"../SceneManager/SceneManager.h"

OverScene::OverScene()
{
}

OverScene::~OverScene()
{
}

void OverScene::Load(void)
{
}

void OverScene::Init(void)
{
}

void OverScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down ||
		KEY::GetIns().GetInfo(KEY_TYPE::GAME_END).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::TITLE);
	}
}

void OverScene::Draw(void)
{
	SetFontSize(32);
	if (KEY::GetIns().GetControllerConnect()) {
		DrawString(10, 0,
			"ゲームオーバー\\n\nタイトルへ：B",
			0xffffff);
	}
	else {
		DrawString(10, 0,
			"ゲームオーバー\\n\nタイトルへ：SPACE",
			0xffffff);
	}
	SetFontSize(16);
}

void OverScene::Release(void)
{
}