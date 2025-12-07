#include"PlayerPunch.h"

#include"../../../Common/Collider/SphereCollider.h"

PlayerPunch::PlayerPunch(const Vector3& playerPos, const Vector3& playerAngle):
	ActorBase(),
	playerPos(playerPos),
	playerAngle(playerAngle)
{
}

void PlayerPunch::Load(void)
{
	ColliderCreate(new SphereCollider(TAG::PLAYER_PUNCH, RADIUS));
}

void PlayerPunch::SubUpdate(void)
{
	if (GetJudgeFlg() == false) { return; }
	trans_.pos = playerPos + LOCAL_POS.TransMat(MGetRotY(playerAngle.y));
}

void PlayerPunch::OnCollision(const ColliderBase& collider)
{
}