#include"GamePauseh.h"

#include"../../../Utility/Utility.h"

#include"../../../Application/Application.h"
#include"../../SceneManager/SceneManager.h"
#include"../../../Manager/Input/KeyManager.h"
#include"../../../Manager/Sound/SoundManager.h"

GamePause::GamePause()
{
}

GamePause::~GamePause()
{
}

void GamePause::Load(void)
{
	img_[(int)SELECT::YES] = Utility::LoadImg("Data/Image/Title/End/Yes.png");
	img_[(int)SELECT::NO] = Utility::LoadImg("Data/Image/Title/End/No.png");
}

void GamePause::Init(void)
{
	nowSelect_ = SELECT::YES;
	SoundManager::GetIns().AllStop();
	KEY::GetIns().SetMouceFixed(false);
}

void GamePause::Update(void)
{
	switch (nowSelect_)
	{
	case GamePause::SELECT::YES:
		if (KEY::GetIns().GetInfo(KEY_TYPE::DOWN).down) { nowSelect_ = GamePause::SELECT::NO; Smng::GetIns().Play(SOUND::SE_SYSTEM_SELECT, true); }
		if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
			Smng::GetIns().PauseInfoDelete();
			Smng::GetIns().Play(SOUND::SE_SYSTEM_BUTTON, true);
			SceneManager::GetIns().JumpScene(SCENE_ID::TITLE);
			return;
		}
		break;
	case GamePause::SELECT::NO:
		if (KEY::GetIns().GetInfo(KEY_TYPE::UP).down) { nowSelect_ = GamePause::SELECT::YES; Smng::GetIns().Play(SOUND::SE_SYSTEM_SELECT, true); }
		if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
			Smng::GetIns().PausePlay();
			Smng::GetIns().Play(SOUND::SE_SYSTEM_BUTTON, true);
			SceneManager::GetIns().PopScene();
			KEY::GetIns().SetMouceFixed(true);
			return;
		}
		break;
	}
	if (KEY::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		SoundManager::GetIns().PausePlay();
		Smng::GetIns().Play(SOUND::SE_SYSTEM_BUTTON, true);
		SceneManager::GetIns().PopScene();
	}
}

void GamePause::Draw(void)
{
	int xx = Application::SCREEN_SIZE_X;
	int yy = Application::SCREEN_SIZE_Y;
	int x = xx / 2;
	int y = yy / 2;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	DrawBox(0, 0, xx, yy, 0xffffff, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawRotaGraph(x, y, 1, 0, img_[(int)nowSelect_], true);
}

void GamePause::Release(void)
{
	for (auto& id : img_) { DeleteGraph(id); }

}