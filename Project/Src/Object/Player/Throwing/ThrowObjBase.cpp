#include"ThrowObjBase.h"

#include"../../../Manager/Collision/CollisionUtility.h"

#include"../../Boss/Boss.h"

ThrowObjBase::ThrowObjBase(const VECTOR& playerPos_, const VECTOR& playerAngle_) :
	playerPos_(playerPos_),
	playerAngle_(playerAngle_),

	model_(-1),

	state_(STATE::NON),
	stateFuncPtr(),

	aliveTime(100),
	aliveCounter_(0),

	ALIVE_HIT_NUM(5),
	aliveHitCou_(0),

	moveVec_()
{
}

ThrowObjBase::~ThrowObjBase()
{
}

void ThrowObjBase::ModelLoad(int model)
{
	unit_.model_ = MV1DuplicateModel(model);
}

void ThrowObjBase::Load(void)
{
}

void ThrowObjBase::Init(void)
{
#pragma region 関数ポインタ配列へ各関数を格納
#define SET_STATE(state, func) stateFuncPtr[(int)(state)] = static_cast<STATEFUNC>(func)
	SET_STATE(STATE::NON, &ThrowObjBase::NonStateFunc);
	SET_STATE(STATE::CARRY, &ThrowObjBase::CarryStateFunc);
	SET_STATE(STATE::DROP, &ThrowObjBase::DropStateFunc);
	SET_STATE(STATE::THROW, &ThrowObjBase::ThrowStateFunc);
#pragma endregion
}

void ThrowObjBase::Update(void)
{
	(this->*stateFuncPtr[(int)state_])();
}

void ThrowObjBase::Draw(void)
{
	if (state_ == STATE::NON) { return; }
	Utility::MV1ModelMatrix(unit_.model_, unit_.pos_, { unit_.angle_ });
	MV1DrawModel(unit_.model_);
}

void ThrowObjBase::Release(void)
{
	MV1DeleteModel(unit_.model_);
}

void ThrowObjBase::OnCollision(UnitBase* other)
{
	if (
		dynamic_cast<Boss*>(other) ||
		dynamic_cast<Stone*>(other) 
		) {
		unit_.isAlive_ = false;
	}

	switch (state_)
	{
	case ThrowObjBase::STATE::NON:
		break;
	case ThrowObjBase::STATE::CARRY:
		break;
	case ThrowObjBase::STATE::DROP:
		break;
	case ThrowObjBase::STATE::THROW:
		if (VoxelBase* voxel = dynamic_cast<RockWall*>(other)) {
			for (const auto& vox : voxel->GetCellCenterPoss()) {
				if (Cfunc::SphereAabb(unit_.pos_, unit_.para_.radius, vox, voxel->GetCellSizeVECTOR())) {
					aliveHitCou_++;
					if (aliveHitCou_ > ALIVE_HIT_NUM) {
						unit_.isAlive_ = false;
					}
				}
			}
		}
		break;
	}

}

void ThrowObjBase::Throw(void)
{
	unit_.pos_ = VAdd(playerPos_, VTransform(LOCAL_THROW_POS, MGetRotY(playerAngle_.y)));

	moveVec_ = VScale(VTransform(THROW_VEC, MGetRotY(playerAngle_.y)), unit_.para_.speed);

	aliveCounter_ = aliveTime;

	aliveHitCou_ = 0;

	state_ = STATE::THROW;
}



void ThrowObjBase::CarryStateFunc(void)
{
	unit_.pos_ = VAdd(playerPos_, VTransform(CARRY_OBJ_LOCAL_POS, Utility::MatrixAllMultY({ playerAngle_ })));
	unit_.angle_ = playerAngle_;
	unit_.isAlive_ = false;
}
void ThrowObjBase::DropStateFunc(void)
{
	unit_.pos_.y -= 5.0f;
	if (unit_.pos_.y < -50.0f) { state_ = STATE::NON; }
	unit_.isAlive_ = false;
}

void ThrowObjBase::ThrowStateFunc(void)
{
	unit_.pos_ = VAdd(unit_.pos_, moveVec_);

	if (--aliveCounter_ <= 0) {
		aliveCounter_ = 0;
		state_ = STATE::NON;
		unit_.isAlive_ = false;
	}
	else { unit_.isAlive_ = true; }
}

