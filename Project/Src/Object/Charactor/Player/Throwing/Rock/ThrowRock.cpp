#include"ThrowRock.h"

#include"../../../../Common/Collider/SphereCollider.h"

ThrowRock::ThrowRock(const int& playerModel, const Vector3& playerAngle) :
	ThrowObjBase(playerModel, playerAngle)
{
}

void ThrowRock::Load(void)
{
	ColliderCreate(new SphereCollider(TAG::PLAYER_THROWING, COLLIDER_RADIUS, COLLIDER_RADIUS));

	speed = 15.0f;

	aliveTime = ALIVE_TIME;

	ALIVE_HIT_NUM = 10;

	trans_.scale = SCALE;
}