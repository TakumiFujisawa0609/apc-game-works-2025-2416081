#include"Boss.h"

#include"../../../Manager/Input/KeyManager.h"
#include"../../../Manager/Sound/SoundManager.h"

#include"../../../Application/Application.h"
#include"../../../Scene/Game/GameScene.h"

#include"../Player/Player.h"

Boss::Boss(const Vector3& playerPos):

	rockWall_(nullptr),
	stone_(nullptr),
	fall_(nullptr),

	attackInterval_(0),
	attackInit_(false),
	attackStart_(false),
	attackEnd_(false),
	spAttackMeasu_(0),

	stanTimer_(0),

	masterLife_(0),

	playerPos(playerPos)
{
}

void Boss::Load(void)
{
	// モデルをロード
	trans_.Load("Boss/Giant");

	// スケールを設定
	trans_.scale = SCALE;

	// 相対　座標/角度
	trans_.centerDiff = -CENTER_DIFF;
	trans_.localAngle = LOCAL_ANGLE;

	SetDynamicFlg(false);
	SetGravityFlg(false);

	SetPushFlg(true);

	// コライダーを生成
	ColliderCreate(new CapsuleCollider(TAG::BOSS, CAPSULE_COLLIDER_START_POS, CAPSULE_COLLIDER_END_POS, CAPSULE_COLLIDER_RADIUS, CAPSULE_COLLIDER_ENOUGH_DISTANCE));

#pragma region 関数ポインタ配列へ各関数を格納

	SET_STATE(STATE::NON, &Boss::Non);
	SET_STATE(STATE::IDLE, &Boss::Idle);
	SET_STATE(STATE::ATTACK, &Boss::Attack);
	SET_STATE(STATE::DAMAGE, &Boss::Damage);
	SET_STATE(STATE::STAN, &Boss::Stan);
	SET_STATE(STATE::BIG_DAMAGE, &Boss::BigDamage);
	SET_STATE(STATE::DEATH, &Boss::Death);
	SET_STATE(STATE::END, &Boss::End);
	
#pragma endregion

	// アニメーション
	CreateAnimationController();
	AnimeLoad();
	AnimePlay((int)ANIME_TYPE::WALK);

	// Bossクラスが抱える子クラス達の読み込み処理
	LowerLoad();

	SetInviEffectFlg(true);
}

void Boss::CharactorInit(void)
{
	state_ = (int)STATE::IDLE;

	trans_.pos = Vector3(1000.0f, 300.0f, 1000.0f);

	SetJudge(true);
	SetIsDraw(true);

	masterLife_ = MASTER_LIFE;
	hp_ = HP_MAX;

	attackInterval_ = ATTACK_INTERVAL[0];

	stanTimer_ = 0;

	// Bossクラスが抱える子クラスの初期化処理
	LowerInit();
}

void Boss::CharactorUpdate(void)
{
	LowerUpdate();
}

void Boss::CharactorDraw(void)
{
	if (!GetIsDraw()) { return; }

	// Bossクラスが抱える子クラスの描画処理
	LowerDraw();
}

void Boss::CharactorAlphaDraw(void)
{
	if (!GetIsDraw()) { return; }

	// Bossクラスが抱える子クラスの描画処理
	LowerAlphaDraw();
}

void Boss::UiDraw(void)
{
	//auto drawHpBar = [&](Vector2 sPos, Vector2 size, int color)->void {
	//	DrawBoxAA(sPos.x, sPos.y, sPos.x + size.x, sPos.y + size.y, color, true);
	//	};

	//float dif = 20.0f;

	//Vector2 size = { Application::SCREEN_SIZE_X * 0.65,60.0f };
	//Vector2 sPos = { Application::SCREEN_SIZE_X - dif - size.x,dif };

	//drawHpBar(sPos, size, 0xffffff);

	//dif = 3.0f;
	//sPos += dif;
	//size -= dif * 2;

	//drawHpBar(sPos, size, 0x000000);

	//size.x *= ((float)hp_ / (float)HP_MAX);
	//drawHpBar(sPos, size, 0xff0000);

	for (unsigned char i = 0; i < masterLife_; i++) { hpBar_[i]->Draw(); }

	if (state_ == (int)STATE::STAN && stanTimer_ / 15 % 2 == 0) {
		SetFontSize(45);
		DrawString(App::SCREEN_SIZE_X/2, 25, "チャンスだ！ぶん殴れ！！", 0xff0000);
		SetFontSize(16);
	}

	DrawFormatString(0, 500, 0xffffff, "ボス座標：X：%.2f,Y：%.2f,Z：%.2f", trans_.pos.x, trans_.pos.y, trans_.pos.z);
}

void Boss::CharactorRelease(void)
{
	// Bossクラスが抱える子クラスの解放処理
	LowerRelease();
}

void Boss::OnCollision(const ColliderBase& collider)
{
	if (GetInviCounter() > 0) { return; }

	if (state_ == (int)STATE::STAN) {
		if (collider.GetTag() == TAG::PLAYER_PUNCH) {
			Smng::GetIns().Play(SOUND::OBJECT_BREAK, true, 150);
			LifeSharpen();
		}
		return;
	}

	if (collider.GetTag() == TAG::PLAYER_THROWING) {
		GameScene::Shake();
		Smng::GetIns().Play(SOUND::OBJECT_BREAK, true, 150);
		HpSharpen(30);
		return;
	}

	if(collider.GetTag()==TAG::PLAYER_PUNCH){
		GameScene::Shake();
		Smng::GetIns().Play(SOUND::OBJECT_BREAK, true, 150);
		HpSharpen(1);
		SetInviCounter(0);
		return;
	}
}

void Boss::Idle(void)
{
	AnimePlay((int)ANIME_TYPE::IDLE);

	Vector3 vec = playerPos - trans_.pos;
	trans_.angle.y = atan2f(vec.x, vec.z);

	if (--attackInterval_ <= 0) {
		attackInterval_ = 0;
		attackInit_ = true;
		state_ = (int)STATE::ATTACK;
	}
}
void Boss::Attack(void)
{
#pragma region 攻撃状態へ遷移後 １回目の処理
	if (attackInit_) {
		attackInit_ = false;
		attackStart_ = true;
		attackEnd_ = false;

		attackState_ = AttackLottery();
		if (attackState_ != ATTACK_KINDS::NON) { attackInterval_ = ATTACK_INTERVAL[(int)attackState_]; }

		switch (attackState_)
		{
		case Boss::ATTACK_KINDS::NON:
			attackEnd_ = true;
			break;
		case Boss::ATTACK_KINDS::FALL:
			AnimePlay((int)ANIME_TYPE::SLAP, false);
			fall_->Set();
			break;
		case Boss::ATTACK_KINDS::STONE:
			AnimePlay((int)ANIME_TYPE::PUNCH, false);
			break;
		case Boss::ATTACK_KINDS::PSYCHO:
			AnimePlay((int)ANIME_TYPE::SLAP, false);
			psycho_->Set();
			break;
		case Boss::ATTACK_KINDS::WALL:
			AnimePlay((int)ANIME_TYPE::SLAP, false);
			break;
		}
	}
#pragma endregion
#pragma region 攻撃開始の処理
	if (attackStart_) {
		switch (attackState_)
		{
		case Boss::ATTACK_KINDS::NON:
			attackStart_ = false;
			break;
		case Boss::ATTACK_KINDS::FALL:
			if (IsAnimeEnd()) {
				attackStart_ = false;
				fall_->On();
			}
			else { return; }
			break;
		case Boss::ATTACK_KINDS::STONE:
			if (IsAnimeEnd()) {
				attackStart_ = false;
				stone_->On();
			}
			else { return; }
			break;
		case Boss::ATTACK_KINDS::PSYCHO:
			if (IsAnimeEnd()) {
				attackStart_ = false;
				psycho_->On();
			}
			else { return; }
			break;
		case Boss::ATTACK_KINDS::WALL:
			if (IsAnimeEnd()) {
				attackStart_ = false;
				rockWall_->On();
			}
			else { return; }
			break;
		}
	}
#pragma endregion
#pragma region 攻撃状態中のみ行う更新処理 また攻撃終了判断
	switch (attackState_)
	{
	case Boss::ATTACK_KINDS::NON:
		attackEnd_ = true;
		break;
	case Boss::ATTACK_KINDS::FALL:
		break;
	case Boss::ATTACK_KINDS::STONE:
		break;
	case Boss::ATTACK_KINDS::PSYCHO:
		break;
	case Boss::ATTACK_KINDS::WALL:
		break;
	}
	if (IsAnimeEnd()) { attackEnd_ = true; }
#pragma endregion
#pragma region 攻撃終了 通常状態へ遷移
	if (attackEnd_) {
		if (attackState_ != ATTACK_KINDS::NON) { attackInterval_ = ATTACK_INTERVAL[(int)attackState_]; }
		state_ = (int)STATE::IDLE;
		AnimePlay((int)ANIME_TYPE::IDLE);
	}
#pragma endregion
}
void Boss::Damage(void)
{
	if (IsAnimeEnd()) {
		state_ = (int)STATE::IDLE;
	}
}
void Boss::Stan(void)
{
	if (--stanTimer_ <= 0) {
		hp_ = (int)(HP_MAX * 0.1f);
		state_ = (int)STATE::IDLE;
	}
}
void Boss::BigDamage(void)
{
	if (GetAnimeRatio() > 0.4f) {
		hp_ = HP_MAX;
		state_ = (int)STATE::IDLE;
		AnimePlay((int)ANIME_TYPE::IDLE);
	}
}
void Boss::Death(void)
{
	if (IsAnimeEnd()) {
		SetJudge(false);
		state_ = (int)STATE::END;
	}
}

Boss::ATTACK_KINDS Boss::AttackLottery(void)
{
	//return ATTACK_KINDS::FALL;
	//return ATTACK_KINDS::STONE;
	//return ATTACK_KINDS::PSYCHO;
	//return ATTACK_KINDS::WALL;

	ATTACK_KINDS ret = ATTACK_KINDS::NON;

	int rand = GetRand(10000);

	if (rand <= 3000) {
		ret = ATTACK_KINDS::FALL;
	}
	else if (rand <= 6000) {
		ret = ATTACK_KINDS::STONE;
	}
	else {
		ret = ATTACK_KINDS::PSYCHO;
	}

	if (++spAttackMeasu_ > SP_ATTACK_MEASU) { spAttackMeasu_ = 0; ret = ATTACK_KINDS::WALL; }

	return ret;
}

void Boss::AnimeLoad(void)
{
	AddInFbxAnimation((int)ANIME_TYPE::MAX, IN_FBX_ANIME_SPEED);

	const std::string ANIME_PATH = "Data/Model/Boss/Animation/";
	//anime_->Add((int)ANIME_TYPE::FALL, 90.0f, (ANIME_PATH + "CarryIdle.mv1").c_str());
}


void Boss::LowerLoad(void)
{
	fall_ = new FallManager(playerPos);
	fall_->Load();

	stone_ = new StoneShooter(trans_.pos, trans_.angle);
	stone_->Load();

	psycho_ = new PsychoRockShooter(trans_.pos, playerPos);
	psycho_->Load();

	rockWall_ = new RockWallShooter(trans_.pos, trans_.angle);
	rockWall_->Load();

	// HPバー
	for (unsigned char i = 0; i < MASTER_LIFE; i++) {
		hpBar_[i] = new BossHpBarManager(hp_, HP_MAX, i);
		hpBar_[i]->Load();
	}
}
void Boss::LowerInit(void)
{
	fall_->Init();
	stone_->Init();
	psycho_->Init();
	rockWall_->Init();
	
	for (unsigned char i = 0; i < MASTER_LIFE; i++) {
		hpBar_[i]->Init(HP_BAR_POS, HP_BAR_COLOR[i]);
	}
}
void Boss::LowerUpdate(void)
{
	fall_->Update();
	stone_->Update();
	psycho_->Update();
	rockWall_->Update();
	
	if (masterLife_ > 0) { hpBar_[masterLife_ - 1]->Update(); }
}
void Boss::LowerDraw(void)
{
	fall_->Draw();
	rockWall_->Draw();
	stone_->Draw();
	psycho_->Draw();
}
void Boss::LowerAlphaDraw(void)
{
	fall_->AlphaDraw();
	rockWall_->AlphaDraw();
	stone_->AlphaDraw();
	psycho_->AlphaDraw();
}
void Boss::LowerRelease(void)
{
	if (rockWall_) {
		rockWall_->Release();
		delete rockWall_;
		rockWall_ = nullptr;
	}
	if (stone_) {
		stone_->Release();
		delete stone_;
		stone_ = nullptr;
	}
	if (fall_) {
		fall_->Release();
		delete fall_;
		fall_ = nullptr;
	}
	if (psycho_) {
		psycho_->Release();
		delete psycho_;
		psycho_ = nullptr;
	}

	for (BossHpBarManager*& h : hpBar_) {
		if (!h) { continue; }
		h->Release();
		delete h;
	}
}

void Boss::HpSharpen(int damage)
{
	if (hp_ <= 0) { return; }

	hp_ -= (hp_ >= damage) ? damage : hp_;

	if (hp_ <= 0) {
		hp_ = 0;

		SetInviCounter(60);

		stanTimer_ = STAN_TIME;

		state_ = (int)STATE::STAN;
		AnimePlay((int)ANIME_TYPE::STAN);
		return;
	}

	if (state_ != (int)STATE::ATTACK) {
		stanTimer_ = STAN_TIME;

		state_ = (int)STATE::DAMAGE;
		AnimePlay((int)ANIME_TYPE::DAMAGE, false);
	}
	SetInviCounter(60);
}
void Boss::LifeSharpen(void)
{
	if (masterLife_ <= 0) { return; }

	if (--masterLife_ <= 0) {
		masterLife_ = 0;
		state_ = (int)STATE::DEATH;
		AnimePlay((int)ANIME_TYPE::DEATH, false);

		GameScene::Slow(100, 10);
		GameScene::Shake(ShakeKinds::WID, ShakeSize::BIG, 100);
		return;
	}

	hp_ = HP_MAX;

	GameScene::Slow(20);
	GameScene::Shake();

	Vector3 vec = playerPos - trans_.pos;
	trans_.angle.y = atan2f(vec.x, vec.z);

	state_ = (int)STATE::BIG_DAMAGE;
	AnimePlay((int)ANIME_TYPE::DEATH, false);
}
