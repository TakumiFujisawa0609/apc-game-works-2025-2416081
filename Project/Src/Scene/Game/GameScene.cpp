#include"GameScene.h"

#include<DxLib.h>
#include<cmath>

#include"../../Application/Application.h"
#include"../../scene/SceneManager/SceneManager.h"

#include"../../Manager/Camera/Camera.h"
#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Sound/SoundManager.h"

#include"../../Utility/Utility.h"

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
	this->Release();

	mainScreen_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);

	camera_ = new Camera();

	collision_ = new Collision();

	blocks_ = new BlockManager();
	blocks_->Load();
	blocks_->SetCamera(camera_);
	collision_->AddStage(blocks_->GetBlocks());

	player_ = new Player(camera_->GetAngles());
	player_->Load();
	collision_->AddDynamicPlayer(player_);
	collision_->AddPlayer(player_->GetSubIns());

	boss_ = new Boss(player_->GetUnit().pos_);
	boss_->Load();
	collision_->AddEnemy(boss_);

	Smng::GetIns().Load(SOUND::OBJECT_BREAK);
}

void GameScene::Init(void)
{
	camera_->Init();
	camera_->ChangeMode(Camera::MODE::FOLLOW);
	camera_->SetLookAtPos(&player_->GetUnit().pos_);

	// ヒットストップカウンターの初期化
	hitStop_ = 0;

	// スローカウンターの初期化
	slow_ = 0;
	slowInter_ = 5;

	// 画面揺れ関係の初期化-----------------------------------------------------------
	shake_ = 0;
	shakeKinds_ = ShakeKinds::DIAG;
	shakeSize_ = ShakeSize::MEDIUM;
	//--------------------------------------------------------------------------------

	player_->Init();
	boss_->Init();
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

	collision_->ResolveDynamics();

	player_->Integrate();
	boss_->Integrate();

	collision_->Check();

	camera_->Update();

	if (KEY::GetIns().GetInfo(KEY_TYPE::GAME_END).down) { App::GetIns().GameEnd(); }
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

	blocks_->Draw();
	boss_->Draw();

	player_->Draw();

	DrawString(0, 0, "ゲーム", 0xffffff);
	//-------------------------------------------------

	SetDrawScreen(DX_SCREEN_BACK);

	Vector2I s = ShakePoint();
	DrawGraph(s.x, s.y, mainScreen_, true);
}

void GameScene::Release(void)
{
	Smng::GetIns().Delete(SOUND::OBJECT_BREAK);
	
	if (blocks_) {
		blocks_->Release();
		delete blocks_;
		blocks_ = nullptr;
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