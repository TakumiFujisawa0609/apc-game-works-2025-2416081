#pragma once

#include"../../VoxelBase.h"

class Block : public VoxelBase
{
public:
	// ブロックモデルの大きさ
	static constexpr float SCALE = 1.0f;

	enum class TYPE
	{
		SIMPLE = (170 << 16) | (170 << 8) | 170,
	};
	// コンストラクタ
	Block(TYPE type, int baseModelId, int textureId, int mapX, int mapY, int mapZ);
	// デストラクタ
	~Block(void)override = default;

	void OnCollision(const ColliderBase& collider)override;

private:
	// ブロック種別
	TYPE type;

	void SubLoad(void)override {}
	void SubInit(void)override { SetJudge(true); SetIsDraw(true); }
};

