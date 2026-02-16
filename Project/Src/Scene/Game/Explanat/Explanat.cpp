#include"Explanat.h"

#include<DxLib.h>

#include"../../../Utility/Utility.h"

#include"../../../Application/Application.h"

#include"../../../Manager/Input/KeyManager.h"
#include"../../../Manager/Sound/SoundManager.h"

#include"../../SceneManager/SceneManager.h"
Explanat::Explanat():
	img(-1),
	bottonImg(),
	s(0.0f)
{
}

Explanat::~Explanat()
{
}

void Explanat::Load(void)
{
	img = LoadImg("Data/Image/Game/Start/Illustrate.png");
	bottonImg[0] = LoadImg("Data/Image/Game/Start/SpaceKey.png");
	bottonImg[1] = LoadImg("Data/Image/Game/Start/X.png");
}

void Explanat::Init(void)
{
	s = 0.0f;
}

void Explanat::Update(void)
{
	if (Key::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetIns().PopScene();
		Snd::GetIns().Play("SystemButton");
		return;
	}
	if (Key::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		SceneManager::GetIns().JumpSceneFade(SCENE_ID::TITLE);
		Snd::GetIns().Play("SystemButton");
		return;
	}

	s += 0.1f;
}

void Explanat::Draw(void)
{
	int xx = Application::SCREEN_SIZE_X;
	int yy = Application::SCREEN_SIZE_Y;
	int x = xx / 2;
	int y = yy / 2;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	DrawBox(0, 0, xx, yy, 0xffffff, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);

	DrawRotaGraph(x, y, 1, 0, img, true);
	DrawRotaGraphF(BUTTON_POS.x, BUTTON_POS.y, GetButtonExrate(), 0, bottonImg[(Key::GetIns().LastInputKinds()) ? 1 : 0], true);
}

void Explanat::Release(void)
{
	for (auto& id : bottonImg) { DeleteGraph(id); }

	DeleteGraph(img);
}