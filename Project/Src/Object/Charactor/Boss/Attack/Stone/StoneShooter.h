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

		for (auto& stone : stones) {
			for (auto& collider : stone->GetCollider()) { ret.emplace_back(collider); }
		}

		return ret;
	}

private:
#pragma region 定数定義
	// 1度に放つ個数
	static constexpr unsigned char ONE_SHOT_NUM = 3;
	// 同時に生成可能な最大数
	static constexpr unsigned char NUM_MAX = ONE_SHOT_NUM * 3;

	// 発生位置(ボス座標からの相対座標)
	const Vector3 LOCAL_POS = { 0.0f,-80.0f,100.0f };
	// 1つ1つの間隔
	const float ONE_DIFF = Deg2Rad(20.0f);
#pragma endregion

	// モデルハンドル
	int model;

	// インスタンス格納配列
	Stone* stones[NUM_MAX];

	// ボス座標(参照用)
	const Vector3& bossPos;
	// ボス角度(参照用)
	const Vector3& bossAngle;
};