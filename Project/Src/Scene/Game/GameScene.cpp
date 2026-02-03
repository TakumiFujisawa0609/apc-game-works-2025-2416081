#include"GameScene.h"

#include<DxLib.h>
#include<cmath>

#include"../../Application/Application.h"
#include"../../scene/SceneManager/SceneManager.h"

#include"../../Manager/Camera/Camera.h"
#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Sound/SoundManager.h"
#include"../../Manager/Score/Score.h"

#include"../../Utility/Utility.h"

#include"Explanat/Explanat.h"

#include"Pause/GamePauseh.h"

#include"../Clear/ClearScene.h"

#include"../../Object/SkyDome/SkyDome.h"
#include"../../Object/UI/Score/ScoreUI.h"
#include"../../Object/Stage/Block/BlockManager.h"
#include"../../Object/Charactor/Player/Player.h"
#include"../../Object/Charactor/Boss/Boss.h"

#include"../../Object/DebugObject/SphereDebugObject.h"
#include"../Debug/GameDebugScene.h"


int GameScene::hitStop_ = 0;

int GameScene::slow_ = 0;
int GameScene::slowInter_ = 0;

int GameScene::shake_ = 0;
ShakeKinds GameScene::shakeKinds_ = ShakeKinds::DIAG;
ShakeSize GameScene::shakeSize_ = ShakeSize::MEDIUM;

GameScene::GameScene():
	collision_(nullptr),

	objects_(),

	mainScreen_(-1),

	timer(0)
{
}

GameScene::~GameScene()
{
}

void GameScene::Load(void)
{
	Snd::GetIns().ChangeScene("Game");

	// 画面演出用
	mainScreen_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);

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
	ObjAdd(new Player());
	ObjAdd(new Boss(ObjSerch<Player>().back()->GetTrans().pos));
	ObjAdd(new ScoreUI());
	//ObjAdd(new SphereDebugObject());
	
	// プレイヤーにリスポーン時ステージ復活の関数を渡す
	ObjSerch<Player>().back()->SetStageRevivalFunc(std::bind(&BlockManager::StageRevival, ObjSerch<BlockManager>().back()));
}

void GameScene::Init(void)
{
	// マウスを中心に固定
	KEY::GetIns().SetMouceFixed(true);

	// オブジェクト全ての初期化処理
	for (ActorBase*& obj : objects_) { obj->Init(); }

	// カメラ設定
	Camera::GetIns().ChangeModeFollowAuto(ObjSerch<Player>().back()->GetTrans(), &(ObjSerch<Boss>().back()->GetTrans().pos));

	// スコアを初期化
	Score::GetIns().Reset();

	timer = 0;

	// イベントシーンをはさむ
	SceneManager::GetIns().PushScene(std::make_shared<Explanat>());

	Snd::GetIns().Play("BatleBgm");
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

	// スコアの更新
	Score::GetIns().Update();

#pragma region 遷移判定（ポーズも含む）
	// ポーズ判定
	if (KEY::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		SceneManager::GetIns().PushScene(std::make_shared<GamePause>());
		return;
	}

	// ゲームクリア判定
	if ((ObjSerch<Boss>().back()->GetState() == (int)Boss::STATE::END)||KEY::GetIns().GetInfo(KEY_TYPE::DEBUG_DRAW_SWITCH).down) {

		// ボーナススコアを追加
		Score::GetIns().ScoreAddBonus(Score::GetIns().BestRecordCombo() * 1000);
		Score::GetIns().ScoreAddBonus((int)(ObjSerch<Player>().back()->HpRatio() * 100000));
		Score::GetIns().ScoreAddBonus(100000 - (timer * 10));

		std::vector<VoxelBase::MeshBatch>meshBatchs = {};
		for (auto& block : ObjSerch<BlockManager>().back()->GetBlocks()) {
			for (auto& mesh : dynamic_cast<VoxelBase*>(block)->GetBatches()) {
				meshBatchs.emplace_back(mesh);
			}
		}
		SceneManager::GetIns().ChangeScene(std::make_shared<ClearScene>(meshBatchs, "Data/Model/Rock/Rock.png"));
		return;
	}
	
	// ゲームオーバー判定
	if (ObjSerch<Player>().back()->GetState() == (int)Player::STATE::END) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::OVER);
		return;
	}

	// デバッグモード突入
	if (KEY::GetIns().GetInfo(KEY_TYPE::DEBUG_MODE_SWITCH).down) {
		SceneManager::GetIns().PushScene(
			std::make_shared<GameDebugScene>(
				[this](void) { Camera::GetIns().ChangeModeFollowAuto(ObjSerch<Player>().back()->GetTrans(), &(ObjSerch<Boss>().back()->GetTrans().pos)); },
				[this](void) { this->Update(); }
			)
		);
		return;
	}
#pragma endregion

	timer++;
}

void GameScene::Draw(void)
{
#pragma region 画面演出
	// 描画先を変更
	SetDrawScreen(mainScreen_);

	// 画面リセット
	ClearDrawScreen();

	// カメラ適用
	Camera::GetIns().Apply();
#pragma endregion

#pragma region 描画処理（メイン）
	// オブジェクト全ての描画処理
	for (ActorBase*& obj : objects_) { obj->Draw(); }
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
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
	// オブジェクト全てのUI描画処理
	for (ActorBase*& obj : objects_) { obj->UiDraw(); }
#pragma endregion
}

void GameScene::Release(void)
{
	Camera::GetIns().Release();

	// 当たり判定管理クラスの解放
	if (collision_) {
		collision_->Clear();
		delete collision_;
		collision_ = nullptr;
	}

	// オブジェクト全ての解放処理
	for (ActorBase*& obj : objects_) {
		if (!obj) { continue; }
		obj->Release();
		delete obj;
		obj = nullptr;
	}

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