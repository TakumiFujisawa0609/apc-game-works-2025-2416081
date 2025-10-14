#include"PlayerGouge.h"

#include"../../Stage/Block/Block.h"
#include"../../Boss/Attack/RockWall/RockWall.h"

PlayerGouge::PlayerGouge(const VECTOR& playerPos, const VECTOR& playerAngle):
	state_(STATE::NON),

	xAngle_(0.0f),

	objectGouge_(false),

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
	objectGouge_ = false;
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
		xAngle_ += Utility::Deg2RadF(5.0f);
		if (xAngle_ > Utility::Deg2RadF(90.0f)) {
			objectGouge_ = false;
			unit_.isAlive_ = false;

			state_ = STATE::NON;
		}
		unit_.pos_ = VAdd(VAdd(playerPos, FOOT_POS), VTransform(LOCAL_POS, Utility::MatrixAllMultXYZ({ {xAngle_,0.0f,0.0f},playerAngle })));
		break;
	case PlayerGouge::STATE::GOUGE:
		if(gougeHit_){
			unit_.isAlive_ = false;

			state_ = STATE::NON;
		}
		break;
	}

	searchHit_ = false;
	gougeHit_ = false;
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
	if (searchHit_) { return; }

	switch (state_)
	{
	case PlayerGouge::STATE::SEARCH:
		if (dynamic_cast<Block*>(other)) {
			state_ = STATE::GOUGE;
			searchHit_ = true;
		}
		if (dynamic_cast<RockWall*>(other)) {
			state_ = STATE::GOUGE;
			searchHit_ = true;
		}
		break;
	case PlayerGouge::STATE::GOUGE:
		if (
			dynamic_cast<Block*>(other) ||
			dynamic_cast<RockWall*>(other)
			) {
			objectGouge_ = true;
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
	objectGouge_ = false;
	state_ = STATE::SEARCH;
	xAngle_ = 0.0f;
}

void PlayerGouge::Off(void)
{
	unit_.isAlive_ = false;
	state_ = STATE::NON;
}
