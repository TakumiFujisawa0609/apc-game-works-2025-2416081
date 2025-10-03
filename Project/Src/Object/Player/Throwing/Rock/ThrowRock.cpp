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
	unit_.para_.radius = 100.0f;

	unit_.para_.speed = 15.0f;

	aliveTime = ALIVE_TIME;
}