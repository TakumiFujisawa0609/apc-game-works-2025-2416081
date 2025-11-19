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
#include"../../Object/Player/Player.h"
#include"../../Object/Boss/Boss.h"

#include"../../Object/Grid/Grid.h"


int GameScene::hitStop_ = 0;

int GameScene::slow_ = 0;
int GameScene::slowInter_ = 0;

int GameScene::shake_ = 0;
ShakeKinds GameScene::shakeKinds_ = ShakeKinds::DIAG;
ShakeSize GameScene::shakeSize_ = ShakeSize::MEDIUM;

GameScene::GameScene():
	mainScreen_(-1),
	camera_(nullptr),
	collision_(nullptr),
	blocks_(nullptr),
	skyDome_(nullptr),
	rock_(),
	player_(nullptr),
	boss_(nullptr)
{
}

GameScene::~GameScene()
{
}

void GameScene::Load(void)
{
	// オブジェクト配列の上限設定(追加時、無駄なメモリ探索をしないように)
	objects_.reserve(10);

	// 初期化も含めたオブジェクト生成のラムダ関数
	auto ObjAdd = [&](ActorBase* newClass)->void {
		objects_.emplace_back(newClass);
		objects_.back()->Init();
		};

 	this->Release();

	mainScreen_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);

	camera_ = new Camera();

	collision_ = new Collision();

	blocks_ = new BlockManager();
	blocks_->Load();
	blocks_->SetCamera(camera_);

	collision_->AddStage(blocks_->GetBlocks());

	skyDome_ = new SkyDome();
	skyDome_->Load();

	player_ = new Player(camera_->GetAngles());
	player_->Load();
	player_->SetStageRevivalFunc(std::bind(&BlockManager::StageRevival, blocks_));
	collision_->AddDynamicPlayer(player_);
	collision_->AddPlayer(player_->GetSubIns());

	boss_ = new Boss(player_->GetUnit().pos_);
	boss_->Load();
	collision_->AddEnemy(boss_);

	Smng::GetIns().Load(SOUND::OBJECT_BREAK);

	KEY::GetIns().SetMouceFixed(true);
}

void GameScene::Init(void)
{
	camera_->Init();
	camera_->ChangeMode(Camera::MODE::FOLLOW);
	camera_->SetLookAtPos(&player_->GetUnit().pos_);
	player_->Init();
	boss_->Init();
	camera_->Update();
	camera_->Update();
	SceneManager::GetIns().PushScene(std::make_shared<Explanat>());
}

void GameScene::Update(void)
{
	if (hitStop_ > 0) { hitStop_--; return; }
	if (shake_ > 0) { shake_--; }
	if (slow_ > 0) {
		slow_--;
		if (slow_ % slowInter_ != 0) { return; }
	}

	player_->Update();
	boss_->Update();
	blocks_->Update();
	skyDome_->Update();

	collision_->ResolveDynamics();

	player_->Integrate();
	boss_->Integrate();

	collision_->Check();

	camera_->Update();

	if (boss_->GetUnit().isAlive_ == false) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::CLEAR);
		return;
	}
	if (player_->GetUnit().isAlive_ == false) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::OVER);
		return;
	}

	if (KEY::GetIns().GetInfo(KEY_TYPE::GAME_END).down) {
		SceneManager::GetIns().PushScene(std::make_shared<GamePause>());
		return;
	}
}

void GameScene::Draw(void)
{
	SetDrawScreen(mainScreen_);
	ClearDrawScreen();
	camera_->Apply();

	//描画処理-----------------------------------------
	using app = Application;
	int x = app::SCREEN_SIZE_X / 2;
	int y = app::SCREEN_SIZE_Y / 2;

	skyDome_->Draw();
	blocks_->Draw();
	player_->Draw();
	boss_->Draw();


	//-------------------------------------------------

	SetDrawScreen(DX_SCREEN_BACK);

	Vector2I s = ShakePoint();
	DrawGraph(s.x, s.y, mainScreen_, true);


	player_->UiDraw();
	boss_->UiDraw();

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
}

void GameScene::Release(void)
{
	Smng::GetIns().Delete(SOUND::OBJECT_BREAK);
	
	if (blocks_) {
		blocks_->Release();
		delete blocks_;
		blocks_ = nullptr;
	}
	if (skyDome_) {
		skyDome_->Release();
		delete skyDome_;
		skyDome_ = nullptr;
	}
	if (player_) {
		player_->Release();
		delete player_;
		player_ = nullptr;
	}
	if (boss_) {
		boss_->Release();
		delete boss_;
		boss_ = nullptr;
	}

	if (collision_) {
		collision_->Clear();
		delete collision_;
		collision_ = nullptr;
	}

	delete camera_;
	camera_ = nullptr;

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