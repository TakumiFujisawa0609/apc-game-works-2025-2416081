#include"TitleScene.h"

#include<DxLib.h>

#include"../../Manager/Input/KeyManager.h"

#include"../../Application/Application.h"
#include"../SceneManager/SceneManager.h"
#include"../../Utility/Utility.h"

#include"End/EndScene.h"


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
	if (KEY::GetIns().GetInfo(KEY_TYPE::GAME_END).down) {
		SceneManager::GetIns().PushScene(std::make_shared<EndScene>());
	}

	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::GAME);
	}
}
void TitleScene::Draw(void)
{
	DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, img_, true);

	SetFontSize(32);
	if (KEY::GetIns().GetControllerConnect()) {
		DrawString(10, 0,
			"タイトル\n\nゲームスタート：B\n\nゲーム終了：START",
			0xffffff);
	} else {
		DrawString(10, 0,
			"タイトル\n\nゲームスタート：SPACE\n\nゲーム終了：ESC",
			0xffffff);
	}
	SetFontSize(16);
}
void TitleScene::Release(void)
{
	DeleteGraph(img_);
}