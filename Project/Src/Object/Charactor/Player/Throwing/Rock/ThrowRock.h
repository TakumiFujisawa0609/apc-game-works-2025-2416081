#pragma once

#include"../ThrowObjBase.h"

class ThrowRock : public ThrowObjBase
{
public:
	ThrowRock(const Vector3& playerPos_, const Vector3& playerAngle_);
	~ThrowRock()override;

	void Load(void)override;

private:
	// 生存時間
	const unsigned short ALIVE_TIME = 300;

	// モデルスケール
	const float SCALE = 1.0f;

	// 半径
	const float COLLIDER_RADIUS = 65.0f * SCALE;
};
