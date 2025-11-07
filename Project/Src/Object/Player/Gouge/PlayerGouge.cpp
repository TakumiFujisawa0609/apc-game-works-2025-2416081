#include"PlayerGouge.h"

#include"../../../Manager/Collision/CollisionUtility.h"

#include"../../Stage/Block/Block.h"
#include"../../Boss/Attack/RockWall/RockWall.h"

PlayerGouge::PlayerGouge(const VECTOR& playerPos, const VECTOR& playerAngle):
	state_(STATE::NON),

	xAngle_(0.0f),

	searchHit_(false),
	gougeHit_(false),

	playerPos(playerPos),
	playerAngle(playerAngle)
{
}

PlayerGouge::~PlayerGouge()
{
}

void PlayerGouge::Load(void)
{
	unit_.para_.colliShape = CollisionShape::SPHERE;
}

void PlayerGouge::Init(void)
{
	searchHit_ = false;
	gougeHit_ = false;
}

void PlayerGouge::Update(void)
{
	if (!unit_.isAlive_) { return; }

	unit_.para_.radius = STATE_RADIUS[(int)state_];

	switch (state_)
	{
	case PlayerGouge::STATE::SEARCH:
		if (searchHit_) { return; }
		xAngle_ += Utility::Deg2RadF(5.0f);
		if (xAngle_ > Utility::Deg2RadF(100.0f)) { xAngle_ = Utility::Deg2RadF(100.0f); }
		else { unit_.pos_ = VAdd(VAdd(playerPos, FOOT_POS), VTransform(LOCAL_POS, Utility::MatrixAllMultXYZ({ {xAngle_,0.0f,0.0f},playerAngle }))); }
		break;
	case PlayerGouge::STATE::GOUGE:
		if(gougeHit_){
			unit_.isAlive_ = false;
			state_ = STATE::NON;
		}
		break;
	}
}

void PlayerGouge::Draw(void)
{
	if (!unit_.isAlive_) { return; }

	DrawSphere3D(unit_.pos_, unit_.para_.radius, 4, 0x000000, 0x000000, true);
}

void PlayerGouge::Release(void)
{
}

void PlayerGouge::OnCollision(UnitBase* other)
{
	switch (state_)
	{
	case PlayerGouge::STATE::SEARCH:
		if (searchHit_) { return; }
		if (
			dynamic_cast<Block*>(other) ||
			dynamic_cast<RockWall*>(other)
			) {
			for (auto v : dynamic_cast<VoxelBase*>(other)->GetCellCenterWorldPoss()) {
				if (Cfunc::Sphere(unit_.pos_, unit_.para_.radius, v, dynamic_cast<VoxelBase*>(other)->GetCellSize())) {
					searchHit_ = true;
					return;
				}
			}
		}
		break;
	case PlayerGouge::STATE::GOUGE:
		if (
			dynamic_cast<Block*>(other) ||
			dynamic_cast<RockWall*>(other)
			) {
			gougeHit_ = true;
		}
		break;
	}
}

void PlayerGouge::On(void)
{
	if (unit_.isAlive_) { return; }

	unit_.pos_ = VAdd(playerPos, FOOT_POS);

	unit_.isAlive_ = true;
	searchHit_ = false;
	gougeHit_ = false;
	state_ = STATE::SEARCH;
	xAngle_ = 0.0f;
}

void PlayerGouge::Off(void)
{
	unit_.isAlive_ = false;
	state_ = STATE::NON;
}

bool PlayerGouge::Gouge(void)
{
	if (searchHit_) {
		state_ = STATE::GOUGE;
		gougeHit_ = false;
		return true;
	}

	Off();
	return false;
}
