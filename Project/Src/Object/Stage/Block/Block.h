#pragma once

#include"../../VoxelBase.h"

class Block : public VoxelBase
{
public:
	// ブロックモデルの大きさ
	static constexpr float SCALE = 1.0f;

	// ブロック種別
	enum class TYPE
	{
		NONE = -1,
		GRASS,
		METAL,
		ICE
	};

	// コンストラクタ
	Block(TYPE type, int baseModelId, int textureId, int mapX, int mapY, int mapZ);
	// デストラクタ
	~Block(void)override{}

	void OnCollision(const ColliderBase& collider)override;

private:
	// ブロック種別
	TYPE type_;

	void SubLoad(void)override{}
	void SubInit(void)override { SetJudge(true); SetIsDraw(true); }
	void SubUpdate(void)override{}
	void SubDraw(void)override {}
	void SubAlphaDraw(void)override {}
	void SubRelease(void)override {}
};

