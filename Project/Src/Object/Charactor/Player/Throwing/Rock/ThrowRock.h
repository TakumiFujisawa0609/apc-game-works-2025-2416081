#pragma once

#include"../ThrowObjBase.h"

class ThrowRock : public ThrowObjBase
{
public:
	ThrowRock(const int& playerModel, const Vector3& playerAngle);
	~ThrowRock()override = default;

	void Load(void)override;

private:
	// 速度
	const float SPEED = 15.0f;

	// 生存時間
	const unsigned short ALIVE_TIME = 300;

	// 生存可能なヒット数
	const unsigned char ALIVE_HIT_NUM = 10;

	// モデルスケール
	const float SCALE = 1.0f;

	// 半径
	const float COLLIDER_RADIUS = 65.0f * SCALE;
};
