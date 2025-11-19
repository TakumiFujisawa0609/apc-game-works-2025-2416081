#include"PlayerPunch.h"

#include"../../../Common/Collider/ColliderSphere.h"

PlayerPunch::PlayerPunch(const Vector3& playerPos, const Vector3& playerAngle):
	ActorBase(),
	playerPos(playerPos),
	playerAngle(playerAngle)
{
}

void PlayerPunch::Load(void)
{
	ColliderCreate(new ColliderSphere(TAG::PLAYER_PUNCH, RADIUS));
}

void PlayerPunch::SubUpdate(void)
{
	if (GetJudgeFlg() == false) { return; }
	trans_.pos = playerPos + LOCAL_POS.TransMat(MGetRotY(playerAngle.y));
}

void PlayerPunch::OnCollision(const ColliderBase& collider)
{

}