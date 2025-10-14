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
}

void ClearScene::Init(void)
{
}

void ClearScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::TITLE);
	}
}

void ClearScene::Draw(void)
{
	SetFontSize(32);
	DrawString(0, 0, "クリア\n\nタイトルへ：キーボードSPACE or コントローラーB", 0xffffff);
	SetFontSize(16);
}

void ClearScene::Release(void)
{
}