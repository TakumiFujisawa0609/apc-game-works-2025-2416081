#pragma once

#include"../../../ActorBase.h"

class PlayerPunch : public ActorBase
{
public:
	const float RADIUS = 100.0f;
	const Vector3 LOCAL_POS = { 0.0f,0.0f,70.0f };

	PlayerPunch(const Vector3& playerPos, const Vector3& playerAngle);
	~PlayerPunch()override {};

	void Load(void)override;

	void OnCollision(const ColliderBase& collider)override;

	void On(void) { SetJudge(true); SetIsDraw(true); SubUpdate(); }
	void Off(void) { SetJudge(false); SetIsDraw(false); }

private:

	void SubInit(void)override { Off(); }
	void SubUpdate(void)override;

	// プレイヤー座標(参照用)
	const Vector3& playerPos;
	// プレイヤー角度(参照用)
	const Vector3& playerAngle;
};

