#pragma once

#include"../ThrowObjBase.h"

class ThrowRock : public ThrowObjBase
{
public:
	static constexpr int ALIVE_TIME = 300;

	static constexpr float SCALE = 1.0f;

	ThrowRock(const Vector3& playerPos_, const Vector3& playerAngle_);
	~ThrowRock()override;

	void Load(void)override;
};
