#include"ThrowObjBase.h"

#include"../../../../Manager/Collision/CollisionUtility.h"

#include"../../../Boss/Boss.h"

ThrowObjBase::ThrowObjBase(const Vector3& playerPos_, const Vector3& playerAngle_) :
	ActorBase(),

	playerPos_(playerPos_),
	playerAngle_(playerAngle_),

	model_(-1),

	state_(STATE::NON),
	stateFuncPtr(),

	aliveTime(100),
	aliveCounter_(0),

	ALIVE_HIT_NUM(5),
	aliveHitCou_(0),

	speed_(10.0f),

	moveVec_()
{
}

ThrowObjBase::~ThrowObjBase()
{
}

void ThrowObjBase::Load(void)
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
}

void ThrowObjBase::SubUpdate(void)
{
	(this->*stateFuncPtr[(int)state_])();

	switch (state_)
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
	if (tag == TAG::BOSS || tag == TAG::GOLEM_ATTACK_STONE) { state_ == STATE::DROP; }

	switch (state_)
	{
	case ThrowObjBase::STATE::NON:
	case ThrowObjBase::STATE::CARRY:
	case ThrowObjBase::STATE::DROP:
		return;
	case ThrowObjBase::STATE::THROW:
		// 何回か当たったら消える処理をあとで書きます
		return;
	}
}

//void ThrowObjBase::OnCollision(UnitBase* other)
//{
//	if (
//		dynamic_cast<Boss*>(other) ||
//		dynamic_cast<Stone*>(other) 
//		) {
//		unit_.isAlive_ = false;
//	}
//
//	switch (state_)
//	{
//	case ThrowObjBase::STATE::NON:
//		break;
//	case ThrowObjBase::STATE::CARRY:
//		break;
//	case ThrowObjBase::STATE::DROP:
//		break;
//	case ThrowObjBase::STATE::THROW:
//		if (VoxelBase* voxel = dynamic_cast<RockWall*>(other)) {
//			for (const auto& vox : voxel->GetCellCenterPoss()) {
//				if (Cfunc::SphereAabb(unit_.pos_, unit_.para_.radius, vox, voxel->GetCellSizeVECTOR())) {
//					aliveHitCou_++;
//					if (aliveHitCou_ > ALIVE_HIT_NUM) {
//						unit_.isAlive_ = false;
//					}
//				}
//			}
//		}
//		break;
//	}
//
//}

void ThrowObjBase::Throw(void)
{
	trans_.pos = playerPos_ + LOCAL_THROW_POS.TransMat(MGetRotY(playerAngle_.y));

	moveVec_ = THROW_VEC.TransMat(MGetRotY(playerAngle_.y)) * speed_;

	aliveCounter_ = aliveTime;

	aliveHitCou_ = 0;

	state_ = STATE::THROW;
}



void ThrowObjBase::CarryStateFunc(void)
{
	trans_.pos = playerPos_ + CARRY_OBJ_LOCAL_POS.TransMat(Utility::MatrixAllMultY({ playerAngle_ }));
	trans_.angle = playerAngle_;
}
void ThrowObjBase::DropStateFunc(void)
{
	trans_.pos.y -= 5.0f;
	if (trans_.pos.y < -50.0f) { state_ = STATE::NON; }
}

void ThrowObjBase::ThrowStateFunc(void)
{
	trans_.pos += moveVec_;

	if (--aliveCounter_ <= 0) {
		aliveCounter_ = 0;
		state_ = STATE::NON;
	}
}

