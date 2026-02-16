#include"TitleScene.h"

#include<DxLib.h>
#include"../../Utility/Utility.h"

#include"../../Application/Application.h"

#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Camera/Camera.h"
#include"../../Manager/Sound/SoundManager.h"

#include"../SceneManager/SceneManager.h"

#include"End/EndScene.h"

#include"../../Object/SkyDome/SkyDome.h"


TitleScene::TitleScene():
	img(-1),
	pushToImg(-1),

	blinkingCounter(0),
	blinkingSigned(5),

	skyDome(nullptr)
{
}

void TitleScene::Load(void)
{
	// 音声のシーン切り替え
	Snd::GetIns().ChangeScene("Title");

#pragma region 画像の読み込み
	// 背景
	LoadImg(img, "Data/Image/Title/Title.png");

	// プッシュトゥスタート（キーボード版 -> [0] / コントローラー版 -> [1]）
	LoadImg(pushToImg[0], "Data/Image/Title/PushToStart/Keyboard.png");
	LoadImg(pushToImg[1], "Data/Image/Title/PushToStart/Controller.png");
#pragma endregion

	// スカイドームの生成
	skyDome = new SkyDome();
	skyDome->Load();

	Key::GetIns().SetMouceFixed(false);
}
void TitleScene::Init(void)
{
	// カメラの初期化
	Camera::GetIns().ChangeModeFixedPoint(Vector3(), Vector3());

	// スカイドームの初期位置を設定
	skyDome->SetPos(SKY_DOME_POS);

	// 点滅のカウンターの初期化
	blinkingCounter = BLINKING_COUNTER_MIN;
	blinkingSigned = 2;

	Snd::GetIns().Play("TitleBgm");
}
void TitleScene::Update(void)
{
	// ゲーム終了処理
	if (Key::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		Snd::GetIns().Pause();
 		Snd::GetIns().Play("SystemSelect");
		SceneManager::GetIns().PushScene(std::make_shared<EndScene>());
		return;
	}

	// シーン進行処理
	if (Key::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		Snd::GetIns().Play("SystemButton");
		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::RANKING);
		return;
	}

	// スカイドームの更新
	skyDome->Update();

	// 点滅の更新
	blinkingCounter += blinkingSigned;
	if (blinkingCounter <= BLINKING_COUNTER_MIN || blinkingCounter > BLINKING_COUNTER_MAX) { blinkingSigned *= -1; }
}
void TitleScene::Draw(void)
{
	// スカイドームの描画
	skyDome->Draw();

	// 背景の描画
	DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, img, true);

	// プッシュトゥスタートの描画（点滅させる）
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, blinkingCounter);
	DrawRotaGraph(App::SCREEN_SIZE_X / 2, (int)(App::SCREEN_SIZE_Y * 0.92f), 1, 0, GetPushToImg(), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
void TitleScene::Release(void)
{
	// スカイドームの解放
	if (skyDome) {
		skyDome->Release();
		delete skyDome;
		skyDome = nullptr;
	}

	// 画像の解放
	for (int& id : pushToImg) { DeleteGraph(id); }
	DeleteGraph(img);
}

const int& TitleScene::GetPushToImg(void) const
{
	return pushToImg[Key::GetIns().LastInputKinds() ? 1 : 0];
}
