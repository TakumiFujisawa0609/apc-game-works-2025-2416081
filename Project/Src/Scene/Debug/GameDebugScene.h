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

	void Load(void)override { Camera::GetIns().ChangeModeFree(Utility::Deg2RadF(2.0f), 10.0f, Camera::GetIns().GetPos(), Camera::GetIns().GetAngle()); }
	void Init(void) override { Smng::GetIns().AllStop(); }
	void Update(void) override {

		if (KEY::GetIns().GetInfo(KEY_TYPE::DEBUG_MODE_TOPUPDATE).now) { TopUpdate(); Smng::GetIns().PausePlay(); }
		else { Smng::GetIns().AllStop(); }

		if (
			KEY::GetIns().GetInfo(KEY_TYPE::DEBUG_MODE_SWITCH).down ||
			KEY::GetIns().GetInfo(KEY_TYPE::PAUSE).down
			) {
			SceneManager::GetIns().PopScene();
			return;
		}
	}
	void Draw(void) override {
		SetFontSize(20);
		DrawString(0, 0, "デバッグモード", 0xffffff);
		SetFontSize(16);
	}
	void Release(void) override { CameraReset(); Smng::GetIns().PausePlay(); }

private:
	std::function<void(void)>CameraReset;
	std::function<void(void)>TopUpdate;
};