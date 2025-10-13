#include"ThrowRock.h"

ThrowRock::ThrowRock()
{
}

ThrowRock::~ThrowRock()
{
}

void ThrowRock::Load(void)
{
	unit_.para_.colliType = CollisionType::ALLY;
	unit_.para_.colliShape = CollisionShape::SPHERE;
	unit_.para_.radius = 200.0f;
	//unit_.para_.radius = 1000.0f;

	unit_.para_.speed = 15.0f;

	aliveTime = ALIVE_TIME;

	MV1SetScale(unit_.model_, { 2.0f,2.0f,2.0f });
	//MV1SetScale(unit_.model_, { 10.0f,10.0f,10.0f });
}