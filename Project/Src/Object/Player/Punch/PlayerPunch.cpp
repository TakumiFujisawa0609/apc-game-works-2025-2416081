#include"PlayerPunch.h"

PlayerPunch::PlayerPunch(const VECTOR& playerPos, const VECTOR& playerAngle):
	playerPos(playerPos),
	playerAngle(playerAngle)
{
}

void PlayerPunch::Load(void)
{
	unit_.para_.colliType = CollisionType::ALLY;
	unit_.para_.colliShape = CollisionShape::SPHERE;

	unit_.para_.radius = RADIUS;
}

void PlayerPunch::Init(void)
{
	unit_.isAlive_ = false;
}

void PlayerPunch::Update(void)
{
	if (!unit_.isAlive_) { return; }
	
	MATRIX matRot = MGetIdent();
	matRot = MMult(matRot, MGetRotY(playerAngle.y));
	unit_.pos_ = VAdd(playerPos, VTransform(LOCAL_POS, matRot));
}

void PlayerPunch::Draw(void)
{
	if (!unit_.isAlive_) { return; }

	//DrawSphere3D(unit_.pos_, unit_.para_.radius, 30, 0x000000, 0x000000, true);
}

void PlayerPunch::Release(void)
{

}

void PlayerPunch::OnCollision(UnitBase* other)
{

}