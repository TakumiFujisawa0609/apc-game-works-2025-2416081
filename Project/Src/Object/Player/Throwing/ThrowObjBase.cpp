#include"ThrowObjBase.h"

#include"../../../Manager/Collision/CollisionUtility.h"

#include"../../Boss/Boss.h"

ThrowObjBase::ThrowObjBase() :
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
}

void ThrowObjBase::Update(void)
{
	if (!unit_.isAlive_) { return; }

	unit_.pos_ = VAdd(unit_.pos_, moveVec_);

	if (--aliveCounter_ <= 0) {
		aliveCounter_ = 0;
		unit_.isAlive_ = false;
	}
}

void ThrowObjBase::Draw(void)
{
	if (!unit_.isAlive_) { return; }

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
}

void ThrowObjBase::Throw(const VECTOR& pos, const VECTOR& vec)
{
	unit_.pos_ = pos;
	moveVec_ = VScale(vec, unit_.para_.speed);

	aliveCounter_ = aliveTime;

	aliveHitCou_ = 0;

	unit_.isAlive_ = true;
}
