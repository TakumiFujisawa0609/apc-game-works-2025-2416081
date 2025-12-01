#pragma once

#include<vector>
#include"PsychoRock.h"

class PsychoRockShooter
{
public:
	PsychoRockShooter(const VECTOR& bossPos, const VECTOR& playerPos);
	~PsychoRockShooter();

	void Load(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	void Set(void);
	void On(void);

private:
	const int ONE_SHOT_NUM = 4;

	int model_;

	std::vector<PsychoRock*>rocks_;

	void RandPos(float& x, float& z);
	const int CONST_RADIUS_SUB = 300;
	const int RANDAM_RADIUS_MAX = 600;

	const VECTOR& bossPos;
	const VECTOR& playerPos;
};