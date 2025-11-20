#include"ThrowRock.h"

#include"../../../../Common/Collider/ColliderSphere.h"

ThrowRock::ThrowRock(const Vector3& playerPos_, const Vector3& playerAngle_) : 
	ThrowObjBase(playerPos_,playerAngle_)
{
}

ThrowRock::~ThrowRock()
{
}

void ThrowRock::Load(void)
{
	ColliderCreate(new ColliderSphere(TAG::PLAYER_THROWING, 100.0f));

	speed_ = 15.0f;

	aliveTime = ALIVE_TIME;

	ALIVE_HIT_NUM = 100;

	trans_.scale = SCALE;
}