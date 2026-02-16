#include "Application.h"

#include <DxLib.h>

#include"../Manager/FPS/FPS.h"
#include"../Manager/Camera/Camera.h"
#include"../Manager/Input/KeyManager.h"
#include"../Manager/Sound/SoundManager.h"
#include"../Manager/Score/Score.h"
#include"../Manager/Score/Ranking.h"
#include"../Manager/Font/FontManager.h"
#include"../Scene/SceneManager/SceneManager.h"


Application* Application::ins = nullptr;

// コンストラクタ
Application::Application(void) :
	isInitFail(false),
	isReleaseFail(false),

	gameEnd(false),

	fps(nullptr),

	drawDebug(false)
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
	ChangeWindowMode(false);
#ifdef _DEBUG
	ChangeWindowMode(true);
#endif // _DEBUG

	// DxLibの初期化
	isInitFail = false;
	if (DxLib_Init() == -1) { isInitFail = true; return; }

	// 描画先画面を裏にする
	SetDrawScreen(DX_SCREEN_BACK);

	// キー制御初期化
	SetUseDirectInputFlag(true);

	// 入力管理クラスの生成 / 初期化処理
	Key::CreateIns();

	// FPS初期化
	fps = new FPS;
	fps->Init();

	// カメラ
	Camera::CreateIns();

	// ランキング管理クラスの生成 / 初期化処理
	Ranking::CreateIns();

	// スコア管理クラスの生成 / 初期化処理
	Score::CreateIns();

	// サウンド管理クラスの生成 / 初期化処理
	Snd::CreateIns();

	// フォントデータ生成 / 初期化処理
	Font::CreateIns();

	// シーン管理初期化 / 初期化処理
	SceneManager::CreateIns();
}

// ゲームループ
void Application::Run(void)
{
	// ゲームループ
	while (ProcessMessage() == 0 && !gameEnd)
	{
		// フレームレート上限まで経過していないなら再ループさせる
		if (!fps->UpdateFrameRate()) { continue; }

		// 入力管理クラスの更新
		KeyManager::GetIns().Update();

		// シーン管理更新
		SceneManager::GetIns().Update();

		// カメラ更新
		Camera::GetIns().Update();

		// 音声管理クラスの更新
		Snd::GetIns().Update();

		// デバッグ表示切替
		if (Key::GetIns().GetInfo(KEY_TYPE::DEBUG_DRAW_SWITCH).down) { DrawDebugSwitch(); }

		// フレームレート計算
		fps->CalcFrameRate();

		// 背面描画画面をクリア
		ClearDrawScreen();

		// カメラ情報を適用
		Camera::GetIns().Apply();

		// シーン管理描画
		SceneManager::GetIns().Draw();

#ifdef _DEBUG
		// フレームレートデバッグ描画
		fps->DrawFrameRate();
#endif // DEBUG

		// 描画が完了した背面画面を表に持ってくる
		ScreenFlip();
	}
}

// 解放
void Application::Release(void)
{
	// シーン管理解放・削除	
	SceneManager::DeleteIns();

	// フォントデータの削除
	Font::DeleteIns();

	// サウンド管理解放・削除
	Snd::DeleteIns();

	// スコア管理クラスの終了処理/消去
	Score::DeleteIns();

	// ランキング管理クラスの終了処理/消去
	Ranking::DeleteIns();

	// カメラ
	Camera::DeleteIns();

	// フレームレート解放
	delete fps;

	// 入力制御削除
	Key::DeleteIns();

	// DxLib終了
	if (DxLib_End() == -1) { isReleaseFail = true; }
}
