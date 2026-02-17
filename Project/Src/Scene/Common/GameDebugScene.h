#pragma once

#include<functional>

#include"../SceneBase.h"

#include"../SceneManager/SceneManager.h"

#include"../../Manager/Camera/Camera.h"
#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Sound/SoundManager.h"

class GameDebugScene : public SceneBase
{
public:
	GameDebugScene(std::function<void(void)>CameraReset, std::function<void(void)>TopUpdate) : SceneBase(),
		CameraReset(std::move(CameraReset)),
		TopUpdate(std::move(TopUpdate))
	{
	}
	~GameDebugScene() = default;

	void Load(void)override { Camera::GetIns().ChangeModeFree(Deg2Rad(2.0f), 10.0f, Camera::GetIns().GetPos(), Camera::GetIns().GetAngle()); }
	void Init(void) override { Snd::GetIns().AllStop(); }
	void Update(void) override {

		if (Key::GetIns().GetInfo(KEY_TYPE::DEBUG_MODE_TOPUPDATE).now) { TopUpdate(); Snd::GetIns().PausePlay(); }
		else { Snd::GetIns().AllStop(); }

		if (
			Key::GetIns().GetInfo(KEY_TYPE::DEBUG_MODE_SWITCH).down ||
			Key::GetIns().GetInfo(KEY_TYPE::PAUSE).down
			) {
			SceneManager::GetIns().PopScene();
			return;
		}
	}
	void Draw(void) override {
		//SetFontSize(20);
		//DrawString(0, 0, "デバッグモード", 0xffffff);
		//SetFontSize(16);
	}
	void Release(void) override { CameraReset(); Snd::GetIns().PausePlay(); }

private:
	std::function<void(void)>CameraReset;
	std::function<void(void)>TopUpdate;
};