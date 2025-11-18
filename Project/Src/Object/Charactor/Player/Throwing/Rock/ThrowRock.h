#pragma once

#include"../ThrowObjBase.h"

class ThrowRock : public ThrowObjBase
{
public:
	static constexpr int ALIVE_TIME = 300;

	ThrowRock(const VECTOR& playerPos_, const VECTOR& playerAngle_);
	~ThrowRock()override;

	void Load(void)override;
};
