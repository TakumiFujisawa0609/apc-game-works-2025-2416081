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

		for (PsychoRock* const& rock : rocks) {
			for (ColliderBase* const& col : rock->GetCollider()) {
				ret.emplace_back(col);
			}
		}

		return ret;
	}

private:
	// 1回の攻撃で放たれる個数
	static constexpr unsigned char ONE_SHOT_NUM = 4;
	// 同時に存在できる最大数
	static constexpr unsigned char NUM_MAX = ONE_SHOT_NUM * 3;

	// モデルハンドル
	int model;

	// インスタンス格納配列
	PsychoRock* rocks[NUM_MAX];
	
	// ランダムで発生位置を算出する
	void RandPos(float& x, float& z);
	// ランダムの範囲
	const float CONST_RADIUS_SUB = 300.0f;
	const float RANDAM_RADIUS_MAX = 600.0f;

	// ボス座標(参照用)
	const Vector3& bossPos;
	// プレイヤー座標(参照用)
	const Vector3& playerPos;
};