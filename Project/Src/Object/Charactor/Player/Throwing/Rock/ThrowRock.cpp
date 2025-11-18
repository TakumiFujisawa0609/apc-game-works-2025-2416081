#include"ThrowRock.h"

ThrowRock::ThrowRock(const VECTOR& playerPos_, const VECTOR& playerAngle_) : ThrowObjBase(playerPos_,playerAngle_)

{
}

ThrowRock::~ThrowRock()
{
}

void ThrowRock::Load(void)
{
	unit_.para_.colliType = CollisionType::ALLY;
	unit_.para_.colliShape = CollisionShape::SPHERE;
	unit_.para_.radius = 100.0f;
	//unit_.para_.radius = 1000.0f;

	unit_.para_.speed = 15.0f;

	aliveTime = ALIVE_TIME;

	ALIVE_HIT_NUM = 100;

	MV1SetScale(unit_.model_, { 1.0f,1.0f,1.0f });
	//MV1SetScale(unit_.model_, { 10.0f,10.0f,10.0f });
}