#include"CarryObjBaseh.h"

#include"../../../Utility/Utility.h"

#include"../../../Manager/Sound/SoundManager.h"

#include"../../../Scene/Game/GameScene.h"

CarryObjBase::CarryObjBase(int model,const VECTOR& playerPos_, const VECTOR& playerAngle_):
	playerPos_(playerPos_),
	playerAngle_(playerAngle_),
	
	model_(-1)
{
	model_ = MV1DuplicateModel(model);
}

CarryObjBase::~CarryObjBase()
{
}

void CarryObjBase::Init(void)
{
#pragma region 関数ポインタ配列へ各関数を格納
#define SET_STATE(state, func) stateFuncPtr[(int)(state)] = static_cast<STATEFUNC>(func)
	SET_STATE(STATE::NON, &CarryObjBase::Non);
	SET_STATE(STATE::CARRY, &CarryObjBase::Carry);
	SET_STATE(STATE::DROP, &CarryObjBase::Drop);
#pragma endregion
}
void CarryObjBase::Update(void)
{
	(this->*stateFuncPtr[(int)state_])();
}
void CarryObjBase::Draw(void)
{
	if (state_ == STATE::NON) { return; }

	Utility::MV1ModelMatrix(model_, pos_, { angle_ });
	MV1DrawModel(model_);
}
void CarryObjBase::Release(void)
{
	MV1DeleteModel(model_);
}

void CarryObjBase::DropObj(void)
{
	state_ = STATE::DROP;
	Smng::GetIns().Play(SOUND::PLAYER_PUNCH, true, 150);
}
void CarryObjBase::Carry(void)
{
	pos_ = VAdd(playerPos_, VTransform(CARRY_OBJ_LOCAL_POS, Utility::MatrixAllMultY({ playerAngle_ })));
	angle_ = playerAngle_;
}
void CarryObjBase::Drop(void)
{
	pos_.y -= 5.0f;
	if (pos_.y < -50.0f) { state_ = STATE::NON; }
}