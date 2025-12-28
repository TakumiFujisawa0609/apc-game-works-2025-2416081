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

	void On(void) { SetJudge(true); SetIsDraw(true); }
	void Off(void) { SetJudge(false); SetIsDraw(false); }

private:

	void SubInit(void)override { Off(); }
	void SubUpdate(void)override;
	void SubDraw(void)override {}
	void SubAlphaDraw(void)override {}
	void SubRelease(void)override {}

	const Vector3& playerPos;
	const Vector3& playerAngle;
};

