#pragma once

#include"VoxelBreakEffect.h"

class VoxelBreakEffectManager
{
public:
	VoxelBreakEffectManager(const int texture);
	~VoxelBreakEffectManager() = default;

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	// 生成関数
	void Spawn(const Vector3& pos, float cellSize, const Vector3& vel);

private:
	
	// 最大数
	static constexpr unsigned char VOXEL_MAX = 20;

	// セル群
	VoxelBreakEffect* voxels[VOXEL_MAX];

	// テクスチャ
	const int texture;
};