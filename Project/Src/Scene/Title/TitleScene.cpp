#include"TitleScene.h"

#include<DxLib.h>

#include"../../Application/Application.h"
#include"../SceneManager/SceneManager.h"
#include"../../Utility/Utility.h"


TitleScene::TitleScene():
	img_(-1)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Load(void)
{
	//Utility::LoadImg(img_, "Data/Image/Title/Title.png");
}
void TitleScene::Init(void)
{
}
void TitleScene::Update(void)
{
	if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::GAME);
	}
}
void TitleScene::Draw(void)
{
	DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, img_, true);
	DrawString(0, 0, "ƒ^ƒCƒgƒ‹", 0xffffff);
}
void TitleScene::Release(void)
{
	DeleteGraph(img_);
}