#include"ThrowRock.h"

#include"../../../../Common/Collider/SphereCollider.h"

ThrowRock::ThrowRock(const int& playerModel, const Vector3& playerAngle) :
	ThrowObjBase(playerModel, playerAngle)
{
}

void ThrowRock::Load(void)
{
	// コライダー作成
	ColliderCreate(new SphereCollider(TAG::PLAYER_THROWING, COLLIDER_RADIUS, COLLIDER_RADIUS));

	// パラメーター設定

	ThrowObjBase::SPEED = this->SPEED;

	ThrowObjBase::ALIVE_TIME = this->ALIVE_TIME;

	ThrowObjBase::ALIVE_HIT_NUM = this->ALIVE_HIT_NUM;

	trans.scale = SCALE;
}