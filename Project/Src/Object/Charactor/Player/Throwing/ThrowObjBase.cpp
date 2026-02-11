#include"ThrowObjBase.h"

#include"../../Boss/Boss.h"

ThrowObjBase::ThrowObjBase(const Vector3& playerPos_, const Vector3& playerAngle_) :
	ActorBase(),

	playerPos(playerPos_),
	playerAngle(playerAngle_),

	model(-1),

	state(STATE::NON),
	stateFuncPtr(),

	aliveTime(100),
	aliveCounter(0),

	ALIVE_HIT_NUM(5),
	aliveHitCou(0),

	speed(10.0f),

	moveVec()
{
}

void ThrowObjBase::SubInit(void)
{
#pragma region 関数ポインタ配列へ各関数を格納
#define SET_STATE(state, func) stateFuncPtr[(int)(state)] = static_cast<STATEFUNC>(func)
	SET_STATE(STATE::NON, &ThrowObjBase::NonStateFunc);
	SET_STATE(STATE::CARRY, &ThrowObjBase::CarryStateFunc);
	SET_STATE(STATE::DROP, &ThrowObjBase::DropStateFunc);
	SET_STATE(STATE::THROW, &ThrowObjBase::ThrowStateFunc);
#pragma endregion

	state = STATE::NON;

	SetDynamicFlg(true);
	SetGravityFlg(false);

	SetPushFlg(false);
}

void ThrowObjBase::SubUpdate(void)
{
	(this->*stateFuncPtr[(int)state])();

	switch (state)
	{
	case ThrowObjBase::STATE::NON:
		SetIsDraw(false);
		SetJudge(false);
		break;
	case ThrowObjBase::STATE::CARRY:
	case ThrowObjBase::STATE::DROP:
		SetIsDraw(true);
		SetJudge(false);
		break;
	case ThrowObjBase::STATE::THROW:
		SetIsDraw(true);
		SetJudge(true);
	}
}

void ThrowObjBase::OnCollision(const ColliderBase& collider)
{
	TAG tag = collider.GetTag();
	if (
		tag != TAG::BOSS &&
		tag != TAG::GOLEM_ATTACK_STONE
		) 
	{
		return;
	}

	switch (state)
	{
	case ThrowObjBase::STATE::NON:
	case ThrowObjBase::STATE::CARRY:
	case ThrowObjBase::STATE::DROP:
		return;
	case ThrowObjBase::STATE::THROW:
		// 何回か当たったら消える処理をあとで書きます
		if (++aliveHitCou >= ALIVE_HIT_NUM) {
			state = STATE::DROP;
		}
		return;
	}
}

void ThrowObjBase::Throw(void)
{
	trans.pos = playerPos + LOCAL_THROW_POS.TransMat(MGetRotY(playerAngle.y));

	moveVec = THROW_VEC.TransMat(MGetRotY(playerAngle.y)) * speed;

	aliveCounter = aliveTime;

	aliveHitCou = 0;

	state = STATE::THROW;
}

void ThrowObjBase::CarryStateFunc(void)
{
	trans.pos = playerPos + CARRY_OBJ_LOCAL_POS.TransMat(MatrixAllMultY({ playerAngle }));
	trans.angle = playerAngle;
}
void ThrowObjBase::DropStateFunc(void)
{
	trans.pos.y -= 5.0f;
	if (trans.pos.y < -50.0f) { state = STATE::NON; }
}

void ThrowObjBase::ThrowStateFunc(void)
{
	trans.pos += moveVec;

	if (--aliveCounter <= 0) {
		aliveCounter = 0;
		state = STATE::NON;
	}
}