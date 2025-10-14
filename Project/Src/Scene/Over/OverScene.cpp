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
	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::TITLE);
	}
}

void OverScene::Draw(void)
{
	SetFontSize(32);
	DrawString(0, 0, "ゲームオーバー\n\nタイトルへ：キーボードSPACE or コントローラーB", 0xffffff);
	SetFontSize(16);
}

void OverScene::Release(void)
{
}