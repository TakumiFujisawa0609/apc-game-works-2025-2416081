#include"Boss.h"

#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Sound/SoundManager.h"

#include"../../Scene/Game/GameScene.h"

#include"../Player/Player.h"

Boss::Boss(const VECTOR& playerPos):
	state_(Boss::STATE::NON),
	stateFuncPtr(),

	anime_(nullptr),

	rockWall_(nullptr),

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
	unit_.para_.radius = SIZE.y / 2;
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

	unit_.hp_ = 200;

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

void Boss::Release(void)
{
	// Bossクラスが抱える子クラスの解放処理
	SubRelease();

	MV1DeleteModel(unit_.model_);
}

void Boss::OnCollision(UnitBase* other)
{
	if (state_ == STATE::STAN) {
		if (dynamic_cast<PlayerPunch*>(other)) {
			GameScene::Shake();
			Smng::GetIns().Play(SOUND::OBJECT_BREAK, true, 150);

			state_ = STATE::BIG_DAMAGE;
			return;
		}
	}

	if (dynamic_cast<ThrowObjBase*>(other)) {
		GameScene::Shake();
		Smng::GetIns().Play(SOUND::OBJECT_BREAK, true, 150);
		HpSharpen(10);
		return;
	}
}

void Boss::Idle(void)
{
	VECTOR vec = VSub(playerPos, unit_.pos_);
	unit_.angle_.y = atan2f(vec.x, vec.z);


	if (KEY::GetIns().GetInfo(KEY_TYPE::DEBUG_VOXEl_CREATE).down) {
		state_ = STATE::ATTACK;
	}
}
void Boss::Attack(void)
{
	rockWall_->On();
	state_ = STATE::IDLE;
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

}
void Boss::Death(void)
{
	unit_.isAlive_ = false;
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
}
void Boss::SubInit(void)
{
	rockWall_->Init();
}
void Boss::SubUpdate(void)
{
	rockWall_->Update();

}
void Boss::SubDraw(void)
{
	rockWall_->Draw();

}
void Boss::SubRelease(void)
{
	if (rockWall_) {
		rockWall_->Release();
		delete rockWall_;
		rockWall_ = nullptr;
	}
}

void Boss::HpSharpen(int damage)
{
	if (unit_.hp_ <= 0) { return; }

	unit_.hp_ -= damage;

	if (unit_.hp_ <= 0) {
		state_ = STATE::DEATH;

		return;
	}

	state_ = STATE::DAMAGE;
	anime_->Play((int)ANIME_TYPE::DAMAGE, false);
	unit_.inviciCounter_ = 60;
}
