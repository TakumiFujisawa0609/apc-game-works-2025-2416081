#pragma once

#include<vector>
#include"Stone.h"

class StoneShooter
{
public:
	static constexpr int ONE_SHOT_NUM = 3;
	static constexpr int NUM_MAX = ONE_SHOT_NUM * 3;

	StoneShooter(const Vector3& bossPos, const Vector3& bossAngle);
	~StoneShooter();

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void AlphaDraw(void);
	void Release(void);


	void On(void);

private:
	int model_;

	std::vector<Stone*>stones_;

	const Vector3 LOCAL_POS = { 0.0f,-50.0f,100.0f };
	const float ONE_DIFF = Utility::Deg2RadF(20.0f);



	const Vector3& bossPos;
	const Vector3& bossAngle;
};