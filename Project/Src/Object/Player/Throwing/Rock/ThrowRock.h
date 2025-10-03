#pragma once

#include"../ThrowObjBase.h"

class ThrowRock : public ThrowObjBase
{
public:
	static constexpr int ALIVE_TIME = 300;

	ThrowRock();
	~ThrowRock()override;

	void Load(void)override;
};
