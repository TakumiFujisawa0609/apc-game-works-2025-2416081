#pragma once

#include"../ThrowObjBase.h"

class ThrowRock : public ThrowObjBase
{
public:
	ThrowRock(const int& playerModel, const Vector3& playerAngle);
	~ThrowRock()override = default;

	void Load(void)override;

private:
	// 生存時間
	const unsigned short ALIVE_TIME = 300;

	// モデルスケール
	const float SCALE = 1.0f;

	// 半径
	const float COLLIDER_RADIUS = 65.0f * SCALE;
};
