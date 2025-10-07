#pragma once

#include"../../VoxelBase.h"

class Block : public VoxelBase
{
public:
	// ブロックモデルの大きさ
	//static constexpr VECTOR SCALES = { 0.5f, 0.5f, 0.5f };
	static constexpr VECTOR SCALES = { 1.0f, 1.0f, 1.0f };

	// ブロック種別
	enum class TYPE
	{
		NONE = -1,
		GRASS,
		METAL,
		ICE
	};

	// コンストラクタ
	Block(void);
	// デストラクタ
	~Block(void)override{}

	// ブロックを生成する
	void Create(TYPE type, int baseModelId, int mapX, int mapY, int mapZ);

	void OnCollision(UnitBase* other)override;

private:
	// ブロック種別
	TYPE type_;


	void SubLoad(void)override;
	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override;
	void SubRelease(void)override;

};

