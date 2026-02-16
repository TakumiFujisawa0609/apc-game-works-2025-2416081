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
#include"../Common/GameDebugScene.h"


int GameScene::hitStop = 0;

int GameScene::slow = 0;
int GameScene::slowInter = 0;

int GameScene::shake = 0;
ShakeKinds GameScene::shakeKinds = ShakeKinds::DIAG;
ShakeSize GameScene::shakeSize = ShakeSize::MEDIUM;

GameScene::GameScene():
	collision(nullptr),

	objects(),

	mainScreen(-1),

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
	mainScreen = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);

	// 当たり判定管理クラスを生成
	collision = new CollisionManager();

	// オブジェクト配列の上限設定(追加時、無駄なメモリ探索をしないように)
	objects.reserve(10);

	// 初期化も含めたオブジェクト生成のラムダ関数
	auto ObjAdd = [&](ActorBase* newClass)->void {
		// 配列の末尾に追加
		objects.emplace_back(newClass);
		// 共通の読み込み処理
		objects.back()->Load();
		// そのオブジェクトが持つコライダーを管理クラスへ追加する
		collision->Add(objects.back()->GetCollider());
		};


	// オブジェクト生成（生成の順番がそのまま(更新/描画)順）
	ObjAdd(new SkyDome());
	ObjAdd(new BlockManager());
	ObjAdd(new Player());
	ObjAdd(new Boss(ObjSerch<Player>()->GetTrans().pos));
	ObjAdd(new ScoreUI());
	//ObjAdd(new SphereDebugObject());
	
	// プレイヤーにリスポーン時ステージ復活の関数を渡す
	ObjSerch<Player>()->SetStageRevivalFunc(std::bind(&BlockManager::StageRevival, ObjSerch<BlockManager>()));

	// イベントシーンをはさむ
	SceneManager::GetIns().PushScene(std::make_shared<Explanat>());
}

void GameScene::Init(void)
{
	// マウスを中心に固定
	Key::GetIns().SetMouceFixed(true);

	// オブジェクト全ての初期化処理
	for (ActorBase*& obj : objects) { obj->Init(); }

	// カメラ設定
	Camera::GetIns().ChangeModeFollowAuto(ObjSerch<Player>()->GetTrans(), &(ObjSerch<Boss>()->GetTrans().pos));

	// スコアを初期化
	Score::GetIns().Reset();

	timer = 0;

	Snd::GetIns().Play("BatleBgm");
}

void GameScene::Update(void)
{
#pragma region 画面演出
	if (hitStop > 0) { hitStop--; return; }
	if (shake > 0) { shake--; }
	if (slow > 0) {
		slow--;
		if (slow % slowInter != 0) { return; }
	}
#pragma endregion

	// オブジェクト全ての更新処理
	for (ActorBase*& obj : objects) { obj->Update(); }

	// 当たり判定
	collision->Check();

	// スコアの更新
	Score::GetIns().Update();

#pragma region 遷移判定（ポーズも含む）
	// ポーズ判定
	if (Key::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		SceneManager::GetIns().PushScene(std::make_shared<GamePause>());
		return;
	}

	// ゲームクリア判定
	if ((ObjSerch<Boss>()->GetState() == (int)Boss::STATE::END)) {
		// 追加予定で待機していたスコアを全て適用しておく
		ObjSerch<ScoreUI>()->AllAddScoreApply();

		// ボーナススコアを追加
		Score::GetIns().ScoreAddBonus(Score::GetIns().BestRecordCombo() * 1000);
		Score::GetIns().ScoreAddBonus((int)(ObjSerch<Player>()->HpRatio() * 100000));
		Score::GetIns().ScoreAddBonus(100000 - (timer * 10));

		std::vector<VoxelBase::MeshBatch>meshBatchs = ObjSerch<BlockManager>()->GetMesh();
		SceneManager::GetIns().ChangeSceneFade(std::make_shared<ClearScene>(meshBatchs, "Data/Model/Rock/Rock.png"));

		return;
	}
	
	// ゲームオーバー判定
	if (ObjSerch<Player>()->GetState() == (int)Player::STATE::END) {
		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::OVER);
		return;
	}

	// デバッグモード突入
	if (Key::GetIns().GetInfo(KEY_TYPE::DEBUG_MODE_SWITCH).down) {
		SceneManager::GetIns().PushScene(
			std::make_shared<GameDebugScene>(
				[this](void) { Camera::GetIns().ChangeModeFollowAuto(ObjSerch<Player>()->GetTrans(), &(ObjSerch<Boss>()->GetTrans().pos)); },
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
	SetDrawScreen(mainScreen);

	// 画面リセット
	ClearDrawScreen();

	// カメラ適用
	Camera::GetIns().Apply();
#pragma endregion

#pragma region 描画処理（メイン）
	// オブジェクト全ての描画処理
	for (ActorBase*& obj : objects) { obj->Draw(); }
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	for (ActorBase*& obj : objects) { obj->AlphaDraw(); }
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
#pragma endregion

#pragma region 画面演出
	// 描画先を元に戻す
	SetDrawScreen(DX_SCREEN_BACK);

	// 揺れの数値を算出
	Vector2I s = ShakePoint();

	// 揺れの数値分座標をずらして描画
	DrawGraph(s.x, s.y, mainScreen, true);
#pragma endregion

#pragma region UI描画（画面演出をかけないもの）
	// オブジェクト全てのUI描画処理
	for (ActorBase*& obj : objects) { obj->UiDraw(); }
#pragma endregion
}

void GameScene::Release(void)
{
	Camera::GetIns().Release();

	// 当たり判定管理クラスの解放
	if (collision) {
		collision->Clear();
		delete collision;
		collision = nullptr;
	}

	// オブジェクト全ての解放処理
	for (ActorBase*& obj : objects) {
		if (!obj) { continue; }
		obj->Release();
		delete obj;
		obj = nullptr;
	}

	// 画面演出用のメインスクリーンを解放
	DeleteGraph(mainScreen);
}

void GameScene::Shake(ShakeKinds kinds, ShakeSize size, int time)
{
	if ((abs(shake - time) > 10) || shake <= 0)shake = time;
	shakeKinds = kinds;
	shakeSize = size;
}
Vector2I GameScene::ShakePoint(void)
{
	Vector2I ret = {};

	if (shake > 0) {
		int size = shake / 5 % 2;
		size *= 2;
		size -= 1;
		switch (shakeKinds)
		{
		case GameScene::WID:ret.x = size;
			break;
		case GameScene::HIG:ret.y = size;
			break;
		case GameScene::DIAG:ret = size;
			break;
		case GameScene::ROUND:
			size = shake / 3 % 12; size++;
			ret = { (int)(((int)shakeSize * 1.5f) * cos(size * 30.0f)),(int)(((int)shakeSize * 1.5f) * sin(size * 30.0f)) };
			break;
		}

		if (shakeKinds != ShakeKinds::ROUND) { ret *= shakeSize; }

		DrawGraph(0, 0, mainScreen, true);
	}

	return ret;
}