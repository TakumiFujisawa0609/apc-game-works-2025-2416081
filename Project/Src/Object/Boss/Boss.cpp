#include"Boss.h"

#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Sound/SoundManager.h"

#include"../../Application/Application.h"
#include"../../Scene/Game/GameScene.h"

#include"../Player/Player.h"

Boss::Boss(const VECTOR& playerPos):
	state_(Boss::STATE::NON),
	stateFuncPtr(),

	rockWall_(nullptr),
	stone_(nullptr),
	fall_(nullptr),
	attackInterval_(0),
	attackInit_(false),
	attackEnd_(false),
	spAttackMeasu_(0),

	anime_(nullptr),

	playerPos(playerPos)
{
}

Boss::~Boss()
{
}

void Boss::Load(void)
{
	unit_.model_ = MV1LoadModel("Data/Model/Boss/Giant.mv1");
	MV1SetScale(unit_.model_, SCALE);

	unit_.para_.size = SIZE;
	unit_.para_.radius = SIZE.y / 3;
	unit_.para_.colliShape = CollisionShape::SPHERE;

	unit_.para_.speed = 10.0f;

	int mnum = MV1GetMaterialNum(unit_.model_);
	for (int i = 0; i < mnum; ++i) {
		COLOR_F emi = MV1GetMaterialEmiColor(unit_.model_, i);
		DEFAULT_COLOR.emplace_back(emi);
		emi.r = (std::min)(emi.r + 0.3f, 1.0f);
		emi.g = (std::min)(emi.g + 0.3f, 1.0f);
		emi.b = (std::min)(emi.b + 0.3f, 1.0f);
		MV1SetMaterialEmiColor(unit_.model_, i, emi);
	}

#pragma region 関数ポインタ配列へ各関数を格納

#define SET_STATE(state, func) stateFuncPtr[(int)(state)] = static_cast<STATEFUNC>(func)

	SET_STATE(STATE::NON, &Boss::Non);
	SET_STATE(STATE::IDLE, &Boss::Idle);
	SET_STATE(STATE::ATTACK, &Boss::Attack);
	SET_STATE(STATE::DAMAGE, &Boss::Damage);
	SET_STATE(STATE::STAN, &Boss::Stan);
	SET_STATE(STATE::BIG_DAMAGE, &Boss::BigDamage);
	SET_STATE(STATE::DEATH, &Boss::Death);
	
#pragma endregion

	// アニメーション
	AnimeLoad();
	anime_->Play((int)ANIME_TYPE::WALK);

	// Bossクラスが抱える子クラス達の読み込み処理
	SubLoad();
}

void Boss::Init(void)
{
	state_ = STATE::IDLE;

	unit_.pos_ = { 1500.0f,400.0f,1500.0f };

	unit_.isAlive_ = true;

	unit_.hp_ = HP_MAX;

	attackInterval_ = ATTACK_INTERVAL;

	// Bossクラスが抱える子クラスの初期化処理
	SubInit();
}

void Boss::Update(void)
{
	if (!unit_.isAlive_) { return; }

	// 無敵カウンターの更新
	Invi();

	// 前フレームの座標をローカル変数に保持(押し戻し処理に使う)
	BeginFrame();

	// 現在のステートに対応する関数を実行
	(this->*stateFuncPtr[(int)state_])();

	// Bossクラスが抱える子クラスの更新処理
	SubUpdate();

	// 加速度の加算
	AccelUpdate();

	// アニメーションの更新
	anime_->Update();

	// ダメージ演出
	if (unit_.inviciCounter_ > 1) {
		if (unit_.inviciCounter_ / 10 % 2 == 0) {
			for (int i = 0; i < DEFAULT_COLOR.size(); i++) {
				MV1SetMaterialEmiColor(unit_.model_, i, DEFAULT_COLOR[i]);
			}
		}
		else {
			for (int i = 0; i < DEFAULT_COLOR.size(); i++) {
				COLOR_F emi = DEFAULT_COLOR[i];
				emi.r = (std::min)(DEFAULT_COLOR[i].r + 0.6f, 1.0f);
				MV1SetMaterialEmiColor(unit_.model_, i, emi);
			}
		}

	}
	else if (unit_.inviciCounter_ == 1) {
		for (int i = 0; i < DEFAULT_COLOR.size(); i++) {
			MV1SetMaterialEmiColor(unit_.model_, i, DEFAULT_COLOR[i]);
		}
	}
}

void Boss::Draw(void)
{
	if (!unit_.isAlive_) { return; }

	// Bossクラスが抱える子クラスの描画処理
	SubDraw();

	Utility::MV1ModelMatrix(unit_.model_, VSub(unit_.pos_, CENTER_DIFF), { LOCAL_ROT,unit_.angle_ });
	MV1DrawModel(unit_.model_);
}

void Boss::UiDraw(void)
{
	auto drawHpBar = [&](Vector2 sPos, Vector2 size, int color)->void {
		DrawBoxAA(sPos.x, sPos.y, sPos.x + size.x, sPos.y + size.y, color, true);
		};

	float dif = 20.0f;

	Vector2 size = { 1000.0f,60.0f };
	Vector2 sPos = { Application::SCREEN_SIZE_X - dif - size.x,dif };

	drawHpBar(sPos, size, 0xffffff);

	dif = 3.0f;
	sPos += dif;
	size -= dif * 2;

	drawHpBar(sPos, size, 0x000000);

	size.x *= ((float)unit_.hp_ / (float)HP_MAX);
	drawHpBar(sPos, size, 0xff0000);
}

void Boss::Release(void)
{
	// Bossクラスが抱える子クラスの解放処理
	SubRelease();

	MV1DeleteModel(unit_.model_);
}

void Boss::OnCollision(UnitBase* other)
{
	if (state_ == STATE::STAN) {
		if (dynamic_cast<PlayerPunch*>(other) ||
			dynamic_cast<ThrowObjBase*>(other)
			) {
			GameScene::Shake();
			Smng::GetIns().Play(SOUND::OBJECT_BREAK, true, 150);

			HpSharpen(50);
			VECTOR vec = VSub(playerPos, unit_.pos_);
			unit_.angle_.y = atan2f(vec.x, vec.z);
			state_ = STATE::BIG_DAMAGE;
			anime_->Play((int)ANIME_TYPE::DEATH, false);
			return;
		}
	}

	if (dynamic_cast<ThrowObjBase*>(other)) {
		GameScene::Shake();
		Smng::GetIns().Play(SOUND::OBJECT_BREAK, true, 150);
		HpSharpen(30);
		return;
	}
}

void Boss::Idle(void)
{
	anime_->Play((int)ANIME_TYPE::IDLE);

	VECTOR vec = VSub(playerPos, unit_.pos_);
	unit_.angle_.y = atan2f(vec.x, vec.z);

	if (--attackInterval_ <= 0) {
		attackInterval_ = 0;
		attackInit_ = false;
		state_ = STATE::ATTACK;
		anime_->Play((int)ANIME_TYPE::ATTACK, false);
	}
}
void Boss::Attack(void)
{
#pragma region 攻撃状態へ遷移後 １回目の処理
	if (!attackInit_) {
		attackInterval_ = ATTACK_INTERVAL;
		attackInit_ = true;
		attackEnd_ = false;

		attackState_ = AttackLottery();
		switch (attackState_)
		{
		case Boss::ATTACK_KINDS::NON:
			attackEnd_ = true;
			break;
		case Boss::ATTACK_KINDS::FALL:
			fall_->On();
			break;
		case Boss::ATTACK_KINDS::STONE:
			stone_->On();
			break;
		case Boss::ATTACK_KINDS::WALL:
			rockWall_->On();
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
	case Boss::ATTACK_KINDS::WALL:
		break;
	}
	if (anime_->GetAnimEnd()) { attackEnd_ = true; }
#pragma endregion
#pragma region 攻撃終了 通常状態へ遷移
	if (attackEnd_) {
		attackInterval_ = ATTACK_INTERVAL;
		state_ = STATE::IDLE;
		anime_->Play((int)ANIME_TYPE::IDLE);
	}
#pragma endregion
}
void Boss::Damage(void)
{
	if (anime_->GetAnimEnd()) {
		state_ = STATE::IDLE;
	}
}
void Boss::Stan(void)
{

}
void Boss::BigDamage(void)
{
	if (anime_->GetAnimeRatio() > 0.6f) {
		state_ = STATE::IDLE;
		anime_->Play((int)ANIME_TYPE::IDLE);
	}
}
void Boss::Death(void)
{
	if (anime_->GetAnimEnd()) {
		unit_.isAlive_ = false;
	}
}

Boss::ATTACK_KINDS Boss::AttackLottery(void)
{
	ATTACK_KINDS ret = ATTACK_KINDS::NON;

	int rand = GetRand(10000);

	if (rand <= 5000) {
		ret = ATTACK_KINDS::FALL;
	}
	else {
		ret = ATTACK_KINDS::STONE;
	}

	if (++spAttackMeasu_ > SP_ATTACK_MEASU) { spAttackMeasu_ = 0; ret = ATTACK_KINDS::WALL; }

	return ret;
}

void Boss::AnimeLoad(void)
{
	anime_ = new AnimationController(unit_.model_);

	for (int i = 0; i < (int)ANIME_TYPE::MAX; i++) {
		anime_->AddInFbx(i, 30.0f, i);
	}

	const std::string ANIME_PATH = "Data/Model/Boss/Animation/";
	//anime_->Add((int)ANIME_TYPE::FALL, 90.0f, (ANIME_PATH + "Fall.mv1").c_str());
}


void Boss::SubLoad(void)
{
	rockWall_ = new RockWallShooter(unit_.pos_, unit_.angle_);
	rockWall_->Load();

	stone_ = new StoneShooter(unit_.pos_, unit_.angle_);
	stone_->Load();

	fall_ = new FallManager(playerPos);
	fall_->Load();
}
void Boss::SubInit(void)
{
	rockWall_->Init();
	stone_->Init();
	fall_->Init();
}
void Boss::SubUpdate(void)
{
	rockWall_->Update();
	stone_->Update();
	fall_->Update();
}
void Boss::SubDraw(void)
{
	rockWall_->Draw();
	stone_->Draw();
	fall_->Draw();
}
void Boss::SubRelease(void)
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
}

void Boss::HpSharpen(int damage)
{
	if (unit_.hp_ <= 0) { return; }

	bool halfOn = false;
	if ((float)unit_.hp_ / (float)HP_MAX >= 0.5f) { halfOn = true; }

	unit_.hp_ -= damage;

	if (unit_.hp_ <= 0) {
		unit_.hp_ = 0;
		state_ = STATE::DEATH;
		anime_->Play((int)ANIME_TYPE::DEATH, false);
		return;
	}

	state_ = STATE::DAMAGE;
	anime_->Play((int)ANIME_TYPE::DAMAGE, false);
	unit_.inviciCounter_ = 60;

	if (halfOn && (((float)unit_.hp_ / (float)HP_MAX) < 0.5f)) {
		state_ = STATE::STAN;
		anime_->Play((int)ANIME_TYPE::DEATH, false);
	}
}
