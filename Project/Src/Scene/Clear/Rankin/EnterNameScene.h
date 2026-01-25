#pragma once
#include"../../SceneBase.h"

#include"../../../Utility/Utility.h"

#include"../../../Application/Application.h"

#include"../../../Manager/Input/KeyManager.h"
#include"../../../Manager/Font/FontManager.h"
#include"../../../Manager/Score/Ranking.h"
#include"../../SceneManager/SceneManager.h"

class EnterNameScene : public SceneBase
{
public:
	EnterNameScene() = default;
	~EnterNameScene()override = default;

	void Load(void)override {}
	void Init(void)override {
		if (!KEY::GetIns().IsInputText()) { KEY::GetIns().IsInputTextSwitch(); }
		KEY::GetIns().InputText().Reset();
	}
	void Update(void)override {
		if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) { SceneManager::GetIns().PopScene(); return; }
	}
	void Draw(void)override {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
		DrawBox(0, 0, App::SCREEN_SIZE_X, App::SCREEN_SIZE_Y, 0xffffff, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawStringToHandle(
			0, 0,
			WStringToString(KEY::GetIns().InputText().InputText()).c_str(),
			0xffffff, Font::GetIns().GetFont(FontKinds::GOKUSYOU_110)
		);
	}
	void Release(void)override {
		Ranking::GetIns().SetLastAddScoreName(WStringToString(KEY::GetIns().InputText().InputText()));
		KEY::GetIns().InputText().Reset();
	}

};