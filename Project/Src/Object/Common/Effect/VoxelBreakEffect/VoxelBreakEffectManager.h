#pragma once

#include"VoxelBreakEffect.h"

class VoxelBreakEffectManager
{
public:
	VoxelBreakEffectManager();
	~VoxelBreakEffectManager() = default;

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

private:
	
	// Å‘å”
	static constexpr unsigned char VOXEL_MAX = 20;

	// ƒZƒ‹ŒQ
	VoxelBreakEffect* voxels[VOXEL_MAX];


};