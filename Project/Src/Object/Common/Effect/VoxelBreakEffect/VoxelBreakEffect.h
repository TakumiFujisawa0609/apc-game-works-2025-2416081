#pragma once

#include"../../../../Utility/Utility.h"

class VoxelBreakEffect
{
public:
	VoxelBreakEffect(const int texture);
	~VoxelBreakEffect() = default;

	void Init(void);
	void Update(void);
	void Draw(void);

	// 生存判定のゲット関数
	bool IsActive(void)const { return isActive; }

	// 生成関数
	void Spawn(const Vector3& pos, float cellSize, const Vector3& vel) {
		this->pos = pos;
		angle = {};
		this->cellSize = cellSize;
		this->vel = vel;
		aliveTime = ALIVE_TIME;
		isActive = true;
	}

private:

#pragma region 定数定義
	// 頂点数
	static constexpr unsigned char VERTEX_NUM = 24;
	// ポリゴン数
	static constexpr unsigned char POLYGON_NUM = 12;
	// インデックス数
	static constexpr unsigned char INDEX_NUM = POLYGON_NUM * 3;

	// 生存時間
	const unsigned char ALIVE_TIME = 40;
#pragma endregion

	// 生存判定
	bool isActive;

	// 生存時間
	unsigned char aliveTime;

	// 座標
	Vector3 pos;

	// 移動量
	Vector3 vel;

	// 角度
	Vector3 angle;

	// セルサイズ
	float cellSize;

#pragma region 描画情報
	// 頂点配列
	VERTEX3D vertex[VERTEX_NUM];

	// インデックス配列
	const unsigned short INDEX[INDEX_NUM] =
	{
		0,	1,	3,		2,	3,	1,
		4,	5,	7,		6,	7,	5,
		8,	9,	11,		10,	11,	9,
		12,	13,	15,		14,	15,	13,
		16,	17,	19,		18,	19,	17,
		20,	21,	23,		22,	23,	21,
	};

	// テクスチャ
	const int texture;
#pragma endregion

	const Vector3 kDir[VERTEX_NUM] =
	{
		Vector3(-1,1,-1).Normalized(),
		Vector3(1,1,-1).Normalized(),
		Vector3(1,-1,-1).Normalized(),
		Vector3(-1,-1,-1).Normalized(),

		Vector3(1,1,-1).Normalized(),
		Vector3(1,1,1).Normalized(),
		Vector3(1,-1,1).Normalized(),
		Vector3(1,-1,-1).Normalized(),

		Vector3(1,1,1).Normalized(),
		Vector3(-1,1,1).Normalized(),
		Vector3(-1,-1,1).Normalized(),
		Vector3(1,-1,1).Normalized(),

		Vector3(-1,1,1).Normalized(),
		Vector3(-1,1,-1).Normalized(),
		Vector3(-1,-1,-1).Normalized(),
		Vector3(-1,-1,1).Normalized(),

		Vector3(-1,1,1).Normalized(),
		Vector3(1,1,1).Normalized(),
		Vector3(1,1,-1).Normalized(),
		Vector3(-1,1,-1).Normalized(),

		Vector3(-1,-1,-1).Normalized(),
		Vector3(1,-1,-1).Normalized(),
		Vector3(1,-1,1).Normalized(),
		Vector3(-1,-1,1).Normalized(),
	};
};