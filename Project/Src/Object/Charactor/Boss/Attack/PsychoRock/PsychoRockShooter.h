#pragma once

#include<vector>
#include"PsychoRock.h"

class PsychoRockShooter
{
public:
	PsychoRockShooter(const Vector3& bossPos, const Vector3& playerPos);
	~PsychoRockShooter();

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void AlphaDraw(void);
	void Release(void);

	void Set(void);
	void On(void);

	std::vector<ColliderBase*> GetCollider(void)const {
		std::vector<ColliderBase*> ret = {};

		for (PsychoRock* const& rock : rocks_) {
			for (ColliderBase* const& col : rock->GetCollider()) {
				ret.emplace_back(col);
			}
		}

		return ret;
	}

private:
	static constexpr unsigned char ONE_SHOT_NUM = 4;
	static constexpr unsigned char NUM_MAX = ONE_SHOT_NUM * 3;

	int model_;

	PsychoRock* rocks_[NUM_MAX];
	
	void RandPos(float& x, float& z);
	const float CONST_RADIUS_SUB = 300.0f;
	const float RANDAM_RADIUS_MAX = 600.0f;

	const Vector3& bossPos;
	const Vector3& playerPos;
};