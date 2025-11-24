#include"Explanat.h"

#include<DxLib.h>

#include"../../../Utility/Utility.h"

#include"../../../Manager/Input/KeyManager.h"

#include"../../../Application/Application.h"
#include"../../SceneManager/SceneManager.h"

Explanat::Explanat():
	img_(-1),
	bottonImg_(),
	s_(0.0f)
{
}

Explanat::~Explanat()
{
}

void Explanat::Load(void)
{
	img_ = Utility::LoadImg("Data/Image/Game/Start/Start.png");
	bottonImg_[0] = Utility::LoadImg("Data/Image/Game/Start/SpaceKey.png");
	bottonImg_[1] = Utility::LoadImg("Data/Image/Game/Start/Bbutton.png");
}

void Explanat::Init(void)
{
	s_ = 0.0f;
}

void Explanat::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetIns().PopScene();
		return;
	}
	if (KEY::GetIns().GetInfo(KEY_TYPE::GAME_END).down) {
		SceneManager::GetIns().JumpScene(SCENE_ID::TITLE);
		return;
	}


	s_ += 0.1f;
}

void Explanat::Draw(void)
{
	int xx = Application::SCREEN_SIZE_X;
	int yy = Application::SCREEN_SIZE_Y;
	int x = xx / 2;
	int y = yy / 2;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	DrawBox(0, 0, xx, yy, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawExtendGraph(x / 2, y / 2, x + (x / 2), y + (y / 2), img_, true);
	DrawRotaGraph(x, y, 1, 0, img_, true);
	DrawRotaGraph(x + (x / 2), y + (y / 2), abs(sinf(s_)) * 0.1f + 0.3f, 0, bottonImg_[(KEY::GetIns().GetControllerConnect()) ? 1 : 0], true);
}

void Explanat::Release(void)
{
	for (auto& id : bottonImg_) { DeleteGraph(id); }

	DeleteGraph(img_);
}