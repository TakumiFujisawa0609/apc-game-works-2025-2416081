#pragma once

#include"Fall.h"

class FallManager
{
public:
	FallManager(const Vector3& playerPos);
	~FallManager();

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

		for (Fall* const& fall : falls) {
			for(ColliderBase* const& col : fall->GetCollider()) {
				ret.emplace_back(col);
			}
		}
	
		return ret;
	}

private:
	// ターゲット位置から出現位置への相対座標
	const Vector3 LOCAL_POS = { 0.0f,500.0f,0.0f };

	// インスタンス最大数
	static constexpr unsigned char FALL_NUM_MAX = 3;

	// モデルハンドルID
	int model;

	// インスタンス格納配列
	Fall* falls[FALL_NUM_MAX];

	// プレイヤー座標(参照用)
	const Vector3& playerPos;
};