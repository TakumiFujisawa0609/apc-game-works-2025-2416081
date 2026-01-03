#pragma once

#include<vector>
#include"Stone.h"

class StoneShooter
{
public:
	StoneShooter(const Vector3& bossPos, const Vector3& bossAngle);
	~StoneShooter();

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void AlphaDraw(void);
	void Release(void);

	void On(void);

	std::vector<ColliderBase*>GetCollider(void) {
		std::vector<ColliderBase*>ret = {};
		ret.reserve(NUM_MAX);

		for (auto& stone : stones_) {
			for (auto& collider : stone->GetCollider()) { ret.emplace_back(collider); }
		}

		return ret;
	}

private:
#pragma region íËêîíËã`
	static constexpr unsigned char ONE_SHOT_NUM = 3;
	static constexpr unsigned char NUM_MAX = ONE_SHOT_NUM * 3;

	const Vector3 LOCAL_POS = { 0.0f,-80.0f,100.0f };
	const float ONE_DIFF = Utility::Deg2RadF(20.0f);
#pragma endregion

	int model_;

	Stone* stones_[NUM_MAX];

	const Vector3& bossPos;
	const Vector3& bossAngle;
};