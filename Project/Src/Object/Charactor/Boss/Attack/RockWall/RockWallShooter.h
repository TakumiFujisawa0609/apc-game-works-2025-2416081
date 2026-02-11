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
		for (auto& wall : walls) {
			for (auto& collider : wall->GetCollider()) { ret.emplace_back(collider); }
		}
		return ret;
	}

private:
#pragma region 定数定義
	// 1度に放つ個数
	const unsigned char CREATE_WALL_NUM = 4;
	// 1個目の出現位置(ボス座標からの相対座標)
	const Vector3 CREATE_ST_LOCAL_POS = { 0.0f,0.0f,450.0f };
	// 1つ1つの出現位置の間隔
	const float ONE_DISTANCE = 150.0f;
	
	// 生成最大個数
	static constexpr unsigned char WALL_MAX_NUM = 40;
#pragma endregion

	// モデルハンドル
	int modelId;
	// テクスチャハンドル
	int textureId;

	// インスタンス格納配列
	RockWall* walls[WALL_MAX_NUM];

	// ボス座標(参照用)
	const Vector3& bossPos;
	// ボス角度(参照用)
	const Vector3& bossAngle;
};
