#pragma once

#include"../ThrowObjBase.h"

class ThrowRock : public ThrowObjBase
{
public:
	ThrowRock(const Vector3& playerPos_, const Vector3& playerAngle_);
	~ThrowRock()override;

	void Load(void)override;

private:
	const unsigned short ALIVE_TIME = 300;

	const float SCALE = 1.0f;

	const float COLLIDER_RADIUS = 65.0f * SCALE;
};
