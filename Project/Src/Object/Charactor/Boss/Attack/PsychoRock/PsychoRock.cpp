#include"PsychoRock.h"

#include"../../../../Manager/Collision/Collision.h"

PsychoRock::PsychoRock(int model, const VECTOR& playerPos) :
	playerPos(playerPos),

	state_(STATE::NON),
	stateUpdateFuncPtr(),
	stateDrawFuncPtr(),

	preparaTimer_(0),

	targetPos_()
{
	unit_.model_ = MV1DuplicateModel(model);
}

PsychoRock::~PsychoRock()
{
}

void PsychoRock::Load(void)
{
	state_ = STATE::NON;

	unit_.para_.colliShape = CollisionShape::SPHERE;
	unit_.para_.radius = 130.0f;

	MV1SetScale(unit_.model_, Utility::FtoV(1.5f));

	unit_.para_.speed = 30.0f;

#pragma region 関数ポインタ配列へ各関数を格納
#define SET_STATE_UPDATE(state, func) stateUpdateFuncPtr[(int)(state)] = static_cast<STATEFUNC>(func)
	SET_STATE_UPDATE(STATE::NON, &PsychoRock::Non);
	SET_STATE_UPDATE(STATE::RISE_PREPARA, &PsychoRock::RisePreparaUpdate);
	SET_STATE_UPDATE(STATE::RISE, &PsychoRock::RiseUpdate);
	SET_STATE_UPDATE(STATE::SHOT, &PsychoRock::ShotUpdate);

#define SET_STATE_DRAW(state, func) stateDrawFuncPtr[(int)(state)] = static_cast<STATEFUNC>(func)
	SET_STATE_DRAW(STATE::NON, &PsychoRock::Non);
	SET_STATE_DRAW(STATE::RISE_PREPARA, &PsychoRock::RisePreparaDraw);
	SET_STATE_DRAW(STATE::RISE, &PsychoRock::RiseDraw);
	SET_STATE_DRAW(STATE::SHOT, &PsychoRock::ShotDraw);
#pragma endregion
}

void PsychoRock::Update(void)
{
	(this->*stateUpdateFuncPtr[(int)state_])();
}

void PsychoRock::Draw(void)
{
	(this->*stateDrawFuncPtr[(int)state_])();
}

void PsychoRock::Release(void)
{
	MV1DeleteModel(unit_.model_);
}

void PsychoRock::OnCollision(UnitBase* other)
{

}

void PsychoRock::RisePreparaUpdate(void)
{
	// 本来Bossクラスで起動するが、正常に動かなかった場合の例外除去
	// 一定時間で勝手に起動
	if (++preparaTimer_ >= PREPARA_TIME) { On(); }
}

void PsychoRock::RiseUpdate(void)
{
	// 飛ばしている間回転させる見栄え的に
	unit_.angle_.y += Utility::Deg2RadF(1.0f);
	unit_.angle_.x += Utility::Deg2RadF(1.0f);

	// 生成場所から一定の高さまで持ち上げ、Shot状態(プレイヤー座標に向かって飛ばす)に遷移する
	unit_.pos_.y += RISE_SPEED;
	if (unit_.pos_.y >= 800.0f) {

		// 移動ベクトルの取得(スピードも入れておく)
		moveVec_ = VScale(VNorm(VSub(targetPos_, unit_.pos_)), unit_.para_.speed);

		// 状態を遷移
		state_ = STATE::SHOT;
	}
}

void PsychoRock::ShotUpdate(void)
{
	// 飛ばしている間回転させる見栄え的に
	unit_.angle_.y += Utility::Deg2RadF(1.0f);
	unit_.angle_.x += Utility::Deg2RadF(1.0f);

	// 取得しておいた移動ベクトルにそって飛ばす
	unit_.pos_ = VAdd(unit_.pos_, moveVec_);

	// 座標のY軸が0を下回ったら(ステージのさらに下まで到達したら)、終了する
	if (unit_.pos_.y < 0.0f) {
		
		// 状態をNON(何もしない再利用待ち)に遷移させる
		state_ = STATE::NON;

		// 当たり判定の判別に使われているので生存フラグも切っておく
		unit_.isAlive_ = false;
	}

	// ありえないが、移動ベクトルが上や真横を向いてしまう(下方向の成分が０になり永遠に上記の終了条件を満たさない)
	// ことも考慮して一定時間(長めの時間)で例外除去
	if (++preparaTimer_ >= PREPARA_TIME) {
		// 上記終了処理と同一
		state_ = STATE::NON; 
		unit_.isAlive_ = false;
	}
}

void PsychoRock::RisePreparaDraw(void)
{
	if (preparaTimer_ / 10 % 2 == 0) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
		DrawSphere3D(unit_.pos_, unit_.para_.radius, 4, 0xff0000, 0xff0000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void PsychoRock::RiseDraw(void)
{
	Utility::MV1ModelMatrix(unit_.model_, unit_.pos_, { unit_.angle_ });
	MV1DrawModel(unit_.model_);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	DrawCapsule3D(unit_.pos_, targetPos_, unit_.para_.radius, 20, 0xff0000, 0xff0000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void PsychoRock::ShotDraw(void)
{
	Utility::MV1ModelMatrix(unit_.model_, unit_.pos_, { unit_.angle_ });
	MV1DrawModel(unit_.model_);
}

void PsychoRock::Set(float x, float z)
{
	unit_.pos_ = { x,500.0f,z };

	unit_.angle_ = {};

	preparaTimer_ = 0;
	state_ = STATE::RISE_PREPARA;
	unit_.isAlive_ = false;

	while (unit_.pos_.y > 0.0f) {
		unit_.pos_.y -= 10.0f;
		if (Collision::IsStageCollision(unit_.pos_, unit_.para_.radius)) {
			unit_.pos_.y -= unit_.para_.radius;
			break;
		}
	}
}

void PsychoRock::On(void)
{
	state_ = STATE::RISE;
	unit_.isAlive_ = true;
	targetPos_ = playerPos;
	targetPos_.y -= unit_.para_.radius;

	preparaTimer_ = 0;
}