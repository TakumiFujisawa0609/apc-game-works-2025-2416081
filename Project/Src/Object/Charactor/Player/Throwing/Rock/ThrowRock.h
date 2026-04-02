#pragma once

#include"../ThrowObjBase.h"

class ThrowRock : public ThrowObjBase
{
public:
	ThrowRock(const int& playerModel, const Vector3& playerAngle);
	~ThrowRock()override = default;

	void Load(void)override;

private:
	// ”¼Œa
	const float COLLIDER_RADIUS = GetParameter("ColliderRadius");
};
