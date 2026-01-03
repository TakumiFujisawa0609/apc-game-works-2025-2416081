#pragma once


#include"RockWall.h"

class RockWallShooter
{
public:

	RockWallShooter(const Vector3& bossPos, const Vector3& bossAngle);
	~RockWallShooter() {}

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void AlphaDraw(void);
	void Release(void);

	void On(void);

	std::vector<ColliderBase*> GetCollider(void) {
		std::vector<ColliderBase*> ret = {};
		ret.reserve(WALL_MAX_NUM);
		for (auto& wall : walls_) {
			for (auto& collider : wall->GetCollider()) { ret.emplace_back(collider); }
		}
		return ret;
	}

private:
#pragma region íËêîíËã`
	const unsigned char CREATE_WALL_NUM = 4;
	const Vector3 CREATE_ST_LOCAL_POS = { 0.0f,0.0f,450.0f };
	const float ONE_DISTANCE = 150.0f;

	static constexpr unsigned char WALL_MAX_NUM = 40;
#pragma endregion

	int modelId_;
	int textureId_;

	RockWall* walls_[WALL_MAX_NUM];

	const Vector3& bossPos;
	const Vector3& bossAngle;
};
