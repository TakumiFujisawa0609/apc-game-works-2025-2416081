#pragma once

#include<vector>

#include"RockWall.h"

class RockWallShooter
{
public:
	static constexpr int CREATE_WALL_NUM = 4;

	static constexpr VECTOR CREATE_ST_LOCAL_POS = { 0.0f,0.0f,450.0f };

	static constexpr float ONE_DISTANCE = 150.0f;

	RockWallShooter(const VECTOR& bossPos, const VECTOR& bossAngle);
	~RockWallShooter();

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	void On(void);

private:
	int textureId_;

	std::vector<RockWall*> walls_;

	const VECTOR& bossPos;
	const VECTOR& bossAngle;
};
