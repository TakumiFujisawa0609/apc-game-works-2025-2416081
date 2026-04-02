#pragma once

#include"../../../ActorBase.h"

class PlayerPunch : public ActorBase
{
public:
	PlayerPunch(const Vector3& playerPos, const Vector3& playerAngle, float RADIUS, const Vector3& LOCAL_POS);
	~PlayerPunch()override = default;

	void Load(void)override;

	void OnCollision(const ColliderBase& collider)override;

	void On(void) { SetJudge(true); SetIsDraw(true); SubUpdate(); }
	void Off(void) { SetJudge(false); SetIsDraw(false); }

private:

	// 攻撃判定の半径
	const float RADIUS;
	// 攻撃判定のローカル座標
	const Vector3 LOCAL_POS;

	void SubInit(void)override { Off(); }
	void SubUpdate(void)override;

	// プレイヤー座標(参照用)
	const Vector3& playerPos;
	// プレイヤー角度(参照用)
	const Vector3& playerAngle;
};

