#include "Application.h"

#include <DxLib.h>

#include"../Manager/FPS/FPS.h"
#include"../Manager/Camera/Camera.h"
#include"../Manager/Input/KeyManager.h"
#include"../Manager/Sound/SoundManager.h"
#include"../Scene/SceneManager/SceneManager.h"

Application* Application::ins_ = nullptr;

// コンストラクタ
Application::Application(void) :
	isInitFail_(false),
	isReleaseFail_(false),

	gameEnd_(false),

	fps_(nullptr),

	drawDebug_(false)
{
}

// デストラクタ
Application::~Application(void)
{
}

// 初期化
void Application::Init(void)
{
	// アプリケーションの初期設定
	SetWindowText("");

	// ウィンドウ関連
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);	// サイズ変更
	ChangeWindowMode(true);	// false = フルスクリーン

	// DxLibの初期化
	isInitFail_ = false;
	if (DxLib_Init() == -1) { isInitFail_ = true; return; }

	// 描画先画面を裏にする
	SetDrawScreen(DX_SCREEN_BACK);

	// キー制御初期化
	SetUseDirectInputFlag(true);
	KeyManager::CreateIns();

	// カメラ
	Camera::CreateIns();

	// シーン管理初期化
	SceneManager::CreateIns();
	SceneManager::GetIns().Init();

	Smng::CreateIns();
	Smng::GetIns().Load(SOUND::SE_SYSTEM_BUTTON);
	Smng::GetIns().Load(SOUND::SE_SYSTEM_SELECT);

	// FPS初期化
	fps_ = new FPS;
	fps_->Init();
}

// ゲームループ
void Application::Run(void)
{
	// ゲームループ
	while (ProcessMessage() == 0 && !gameEnd_)
	{
		// フレームレート上限まで経過していないなら再ループさせる
		if (!fps_->UpdateFrameRate()) { continue; }

		// 入力管理クラスの更新
		KeyManager::GetIns().Update();

		// シーン管理更新
		SceneManager::GetIns().Update();

		// カメラ更新
		Camera::GetIns().Update();

		// デバッグ表示切替
		if (KEY::GetIns().GetInfo(KEY_TYPE::DEBUG_DRAW_SWITCH).down) { DrawDebugSwitch(); }

		// フレームレート計算
		fps_->CalcFrameRate();

		// 背面描画画面をクリア
		ClearDrawScreen();

		// カメラ情報を適用
		Camera::GetIns().Apply();

		// シーン管理描画
		SceneManager::GetIns().Draw();

#ifdef _DEBUG

		// フレームレートデバッグ描画
		fps_->DrawFrameRate();

		// カメラデバッグ描画
		//Camera::GetIns().DrawDebug();

#endif // DEBUG


		// 描画が完了した背面画面を表に持ってくる
		ScreenFlip();
	}
}

// 解放
void Application::Release(void)
{
	// 入力制御削除
	KeyManager::DeleteIns();

	Smng::DeleteIns();

	// シーン管理解放・削除	
	SceneManager::GetIns().Release();
	SceneManager::DeleteIns();

	// カメラ
	Camera::DeleteIns();

	// フレームレート解放
	delete fps_;

	// DxLib終了
	if (DxLib_End() == -1) { isReleaseFail_ = true; }
}
