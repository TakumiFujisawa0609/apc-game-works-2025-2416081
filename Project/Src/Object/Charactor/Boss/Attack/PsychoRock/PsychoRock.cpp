#include"PsychoRock.h"

#include"../../../../../Manager/Collision/CollisionManager.h"

#include"../../../../Common/Collider/SphereCollider.h"

PsychoRock::PsychoRock(int model, const Vector3& playerPos) :
	playerPos(playerPos),

	state_(STATE::NON),
	stateUpdateFuncPtr(),
	stateDrawFuncPtr(),

	preparaTimer_(0),

	targetPos_()
{
	ModelDuplicate(model);
}

PsychoRock::~PsychoRock()
{
}

void PsychoRock::Load(void)
{
	state_ = STATE::NON;

	trans_.scale = SCALE;

	ColliderCreate(new SphereCollider(TAG::GOLEM_ATTACK_PSYCHOROCK, RADIUS, RADIUS));

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

void PsychoRock::SubInit(void)
{
	SetDynamicFlg(true);
	SetGravityFlg(false);

	SetPushFlg(false);
}

void PsychoRock::SubUpdate(void)
{
	(this->*stateUpdateFuncPtr[(int)state_])();
}

void PsychoRock::SubAlphaDraw(void)
{
	(this->*stateDrawFuncPtr[(int)state_])();
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
	trans_.AddAngleXDeg(1.0f);
	trans_.AddAngleYDeg(1.0f);

	// 生成場所から一定の高さまで持ち上げ、Shot状態(プレイヤー座標に向かって飛ばす)に遷移する
	trans_.pos.y += RISE_SPEED;
	if (trans_.pos.y >= 800.0f) {

		// 移動ベクトルの取得(スピードも入れておく)
		moveVec_ = (targetPos_ - trans_.pos).Normalized() * MOVE_SPEED;

		// 状態を遷移
		state_ = STATE::SHOT;
	}
}

void PsychoRock::ShotUpdate(void)
{
	// 飛ばしている間回転させる見栄え的に
	trans_.AddAngleXDeg(1.0f);
	trans_.AddAngleYDeg(1.0f);

	// 取得しておいた移動ベクトルにそって飛ばす
	trans_.pos += moveVec_;

	// 座標のY軸が0を下回ったら(ステージのさらに下まで到達したら)、終了する
	if (trans_.pos.y < 0.0f) {
		
		// 状態をNON(何もしない再利用待ち)に遷移させる
		state_ = STATE::NON;

		// 当たり判定の判別に使われているので生存フラグも切っておく
		SetJudge(false);
		SetIsDraw(false);
	}

	// ありえないが、移動ベクトルが上や真横を向いてしまう(下方向の成分が０になり永遠に上記の終了条件を満たさない)
	// ことも考慮して一定時間(長めの時間)で例外除去
	if (++preparaTimer_ >= PREPARA_TIME) {
		// 上記終了処理と同一
		state_ = STATE::NON; 
		SetJudge(false);
		SetIsDraw(false);
	}
}

void PsychoRock::RisePreparaDraw(void)
{
	SetIsDraw(false);

	if (preparaTimer_ / 10 % 2 == 0) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
		DrawSphere3D(trans_.pos.ToVECTOR(), ColliderSerch<SphereCollider>().back()->GetRadius(), 4, 0xff0000, 0xff0000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void PsychoRock::RiseDraw(void)
{
	SetIsDraw(true);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	DrawCapsule3D(trans_.pos.ToVECTOR(), targetPos_.ToVECTOR(), ColliderSerch<SphereCollider>().back()->GetRadius(), 20, 0xff0000, 0xff0000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void PsychoRock::ShotDraw(void)
{
	SetIsDraw(true);
}

void PsychoRock::Set(float x, float z)
{
	trans_.pos = Vector3(x, 500.0f, z);

	trans_.angle = {};

	preparaTimer_ = 0;
	state_ = STATE::RISE_PREPARA;
	SetJudge(false);
	SetIsDraw(false);

	float radius = ColliderSerch<SphereCollider>().back()->GetRadius();

	while (trans_.pos.y > 0.0f) {
		trans_.pos.y -= 10.0f;
		if (CollisionManager::IsStageCollision(trans_.pos, radius)) {
			trans_.pos.y -= radius;
			break;
		}
	}
}

void PsychoRock::On(void)
{
	state_ = STATE::RISE;
	SetJudge(true);
	SetIsDraw(true);
	targetPos_ = playerPos;
	targetPos_.y -= ColliderSerch<SphereCollider>().back()->GetRadius();

	preparaTimer_ = 0;
}