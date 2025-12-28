#include"GameScene.h"

#include<DxLib.h>
#include<cmath>

#include"../../Application/Application.h"
#include"../../scene/SceneManager/SceneManager.h"

#include"../../Manager/Camera/Camera.h"
#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Sound/SoundManager.h"

#include"../../Utility/Utility.h"

#include"Explanat/Explanat.h"

#include"Pause/GamePauseh.h"

#include"../../Object/SkyDome/SkyDome.h"
#include"../../Object/Stage/Block/BlockManager.h"
#include"../../Object/Charactor/Player/Player.h"
#include"../../Object/Charactor/Boss/Boss.h"

#include"../../Object/DebugObject/SphereDebugObject.h"


int GameScene::hitStop_ = 0;

int GameScene::slow_ = 0;
int GameScene::slowInter_ = 0;

int GameScene::shake_ = 0;
ShakeKinds GameScene::shakeKinds_ = ShakeKinds::DIAG;
ShakeSize GameScene::shakeSize_ = ShakeSize::MEDIUM;

GameScene::GameScene():
	camera_(nullptr),
	collision_(nullptr),

	objects_(),

	mainScreen_(-1)
{
}

GameScene::~GameScene()
{
}

void GameScene::Load(void)
{
	// 画面演出用
	mainScreen_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);

	// カメラを生成
	camera_ = new Camera();

	// 当たり判定管理クラスを生成
	collision_ = new CollisionManager();


	// オブジェクト配列の上限設定(追加時、無駄なメモリ探索をしないように)
	objects_.reserve(10);

	// 初期化も含めたオブジェクト生成のラムダ関数
	auto ObjAdd = [&](ActorBase* newClass)->void {
		// 配列の末尾に追加
		objects_.emplace_back(newClass);
		// 共通の読み込み処理
		objects_.back()->Load();
		// そのオブジェクトが持つコライダーを管理クラスへ追加する
		collision_->Add(objects_.back()->GetCollider());
		};


	// オブジェクト生成（生成の順番がそのまま(更新/描画)順）
	ObjAdd(new SkyDome());
	ObjAdd(new BlockManager());
	ObjAdd(new Player(camera_->GetAngles()));
	ObjAdd(new Boss(ObjSerch<Player>().back()->GetTrans().pos));
	//ObjAdd(new SphereDebugObject(camera_->GetAngles()));

	// ステージにカメラを渡す
	ObjSerch<BlockManager>().back()->SetCamera(camera_);

	// プレイヤーにリスポーン時ステージ復活の関数を渡す
	ObjSerch<Player>().back()->SetStageRevivalFunc(std::bind(&BlockManager::StageRevival, ObjSerch<BlockManager>().back()));

#pragma region ゲームシーンで使用するサウンドをロード
	Smng::GetIns().Load(SOUND::OBJECT_BREAK);
#pragma endregion
}

void GameScene::Init(void)
{
	// オブジェクト全ての初期化処理
	for (ActorBase*& obj : objects_) { obj->Init(); }

	// カメラ初期化～～～～～～～～～～～～～～～～～～～～～～～～～～～～～
	camera_->ChangeMode(Camera::MODE::FOLLOW);
	camera_->SetLookAtPos(&(ObjSerch<Player>().back()->GetTrans().pos));
	camera_->Init();
	// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

	// マウスを中心に固定
	KEY::GetIns().SetMouceFixed(true);

	// イベントシーンをはさむ
	SceneManager::GetIns().PushScene(std::make_shared<Explanat>());
}

void GameScene::Update(void)
{
#pragma region 画面演出
	if (hitStop_ > 0) { hitStop_--; return; }
	if (shake_ > 0) { shake_--; }
	if (slow_ > 0) {
		slow_--;
		if (slow_ % slowInter_ != 0) { return; }
	}
#pragma endregion

	// オブジェクト全ての更新処理
	for (ActorBase*& obj : objects_) { obj->Update(); }

	// 当たり判定
	collision_->Check();

	// カメラ更新
	camera_->Update();

#pragma region 遷移判定（ポーズも含む）
	// ポーズ判定
	if (KEY::GetIns().GetInfo(KEY_TYPE::GAME_END).down) {
		SceneManager::GetIns().PushScene(std::make_shared<GamePause>());
		return;
	}

	// ゲームクリア判定
	//if () {
	//	SceneManager::GetIns().ChangeScene(SCENE_ID::CLEAR);
	//	return;
	//}
	
	// ゲームオーバー判定
	//if () {
	//	SceneManager::GetIns().ChangeScene(SCENE_ID::OVER);
	//	return;
	//}
#pragma endregion
}

void GameScene::Draw(void)
{
#pragma region 画面演出
	// 描画先を変更
	SetDrawScreen(mainScreen_);

	// 画面リセット
	ClearDrawScreen();

	// カメラ適用
	camera_->Apply();
#pragma endregion

#pragma region 描画処理（メイン）
	// オブジェクト全ての描画処理
	for (ActorBase*& obj : objects_) { obj->Draw(); }
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	for (ActorBase*& obj : objects_) { obj->AlphaDraw(); }
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#pragma endregion

#pragma region 画面演出
	// 描画先を元に戻す
	SetDrawScreen(DX_SCREEN_BACK);

	// 揺れの数値を算出
	Vector2I s = ShakePoint();

	// 揺れの数値分座標をずらして描画
	DrawGraph(s.x, s.y, mainScreen_, true);
#pragma endregion

#pragma region UI描画（画面演出をかけないもの）

	ObjSerch<Player>().back()->UiDraw();
	ObjSerch<Boss>().back()->UiDraw();

	if (KEY::GetIns().GetControllerConnect()) {
		DrawString(10, 0, 
			"\nコントローラー操作方法\n\n移動：左スティック or 十字ボタン\n\nジャンプ：A\n\nパンチ：X or RT\n\nつかむ：RB or Y長押し\n\n投げる：X or RT\n\n回避：LT or B\n\nカメラ操作：右スティック\n\nポーズ：START",
			0xffffff);
	}
	else {
		DrawString(10, 0,
			"\nキーボード操作方法\n\n移動：WASD\n\nジャンプ：SPACE\n\nパンチ：左クリック or J\n\nつかむ：右クリック or K  長押し\n\n投げる：左クリック or J\n\n回避：左Shift or H\n\nカメラ操作：マウス or 十字キー\n\nポーズ：ESC",
			0xffffff);
	}

#pragma endregion
}

void GameScene::Release(void)
{
	Smng::GetIns().Delete(SOUND::OBJECT_BREAK);
	
	// 当たり判定管理クラスの解放
	if (collision_) {
		collision_->Clear();
		delete collision_;
		collision_ = nullptr;
	}

	// オブジェクト全ての解放処理
	for (ActorBase*& obj : objects_) { obj->Release(); }

	// カメラを解放
	delete camera_;
	camera_ = nullptr;

	// 画面演出用のメインスクリーンを解放
	DeleteGraph(mainScreen_);
}

void GameScene::Shake(ShakeKinds kinds, ShakeSize size, int time)
{
	if ((abs(shake_ - time) > 10) || shake_ <= 0)shake_ = time;
	shakeKinds_ = kinds;
	shakeSize_ = size;
}
Vector2I GameScene::ShakePoint(void)
{
	Vector2I ret = {};

	if (shake_ > 0) {
		int size = shake_ / 5 % 2;
		size *= 2;
		size -= 1;
		switch (shakeKinds_)
		{
		case GameScene::WID:ret.x = size;
			break;
		case GameScene::HIG:ret.y = size;
			break;
		case GameScene::DIAG:ret = size;
			break;
		case GameScene::ROUND:
			size = shake_ / 3 % 12; size++;
			ret = { (int)(((int)shakeSize_ * 1.5f) * cos(size * 30.0f)),(int)(((int)shakeSize_ * 1.5f) * sin(size * 30.0f)) };
			break;
		}

		if (shakeKinds_ != ShakeKinds::ROUND) { ret *= shakeSize_; }

		DrawGraph(0, 0, mainScreen_, true);
	}

	return ret;
}