#include"ThrowRock.h"

#include"../../../../Common/Collider/SphereCollider.h"

ThrowRock::ThrowRock(const int& playerModel, const Vector3& playerAngle, int PLAYER_RIGHTHAND_FRAME_INDEX) :
	ThrowObjBase(playerModel, playerAngle, PLAYER_RIGHTHAND_FRAME_INDEX, "Data/Parameter/Player/Throwing/ThrowRockParameter.csv")
{
}

void ThrowRock::Load(void)
{
	// コライダー作成
	ColliderCreate(new SphereCollider(TAG::PLAYER_THROWING, COLLIDER_RADIUS, COLLIDER_RADIUS));
}