#include"GamePauseh.h"

#include"../../../Utility/Utility.h"

#include"../../../Application/Application.h"
#include"../../SceneManager/SceneManager.h"
#include"../../../Manager/Input/KeyManager.h"
#include"../../../Manager/Sound/SoundManager.h"

GamePause::GamePause() :
	img(),
	nowSelect(SELECT::YES)
{
}

GamePause::~GamePause()
{
}

void GamePause::Load(void)
{
	img[(int)SELECT::YES] = LoadImg("Data/Image/Title/End/Yes.png");
	img[(int)SELECT::NO] = LoadImg("Data/Image/Title/End/No.png");
}

void GamePause::Init(void)
{
	nowSelect = SELECT::YES;
	SoundManager::GetIns().AllStop();
	Key::GetIns().SetMouceFixed(false);
}

void GamePause::Update(void)
{
	switch (nowSelect)
	{
	case GamePause::SELECT::YES:
		if (Key::GetIns().GetInfo(KEY_TYPE::DOWN).down) { nowSelect = GamePause::SELECT::NO; Snd::GetIns().Play("SystemSelect"); }
		if (Key::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
			Snd::GetIns().Play("SystemButton");
			SceneManager::GetIns().JumpSceneFade(SCENE_ID::TITLE);
			return;
		}
		break;
	case GamePause::SELECT::NO:
		if (Key::GetIns().GetInfo(KEY_TYPE::UP).down) { nowSelect = GamePause::SELECT::YES; Snd::GetIns().Play("SystemSelect"); }
		if (Key::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
			Snd::GetIns().PausePlay();
			Snd::GetIns().Play("SystemButton");
			SceneManager::GetIns().PopScene();
			Key::GetIns().SetMouceFixed(true);
			return;
		}
		break;
	}
	if (Key::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		SoundManager::GetIns().PausePlay();
		Snd::GetIns().Play("SystemButton");
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

	DrawRotaGraph(x, y, 1, 0, img[(int)nowSelect], true);
}

void GamePause::Release(void)
{
	for (auto& id : img) { DeleteGraph(id); }

}