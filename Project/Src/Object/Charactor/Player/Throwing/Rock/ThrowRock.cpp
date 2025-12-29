#include"ThrowRock.h"

#include"../../../../Common/Collider/SphereCollider.h"

ThrowRock::ThrowRock(const Vector3& playerPos_, const Vector3& playerAngle_) : 
	ThrowObjBase(playerPos_,playerAngle_)
{
}

ThrowRock::~ThrowRock()
{
}

void ThrowRock::Load(void)
{
	ColliderCreate(new SphereCollider(TAG::PLAYER_THROWING, COLLIDER_RADIUS, COLLIDER_RADIUS));

	speed_ = 15.0f;

	aliveTime = ALIVE_TIME;

	ALIVE_HIT_NUM = 10;

	trans_.scale = SCALE;
}