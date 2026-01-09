#include"VoxelBreakEffectManager.h"

VoxelBreakEffectManager::VoxelBreakEffectManager() :
	voxels()
{
}

void VoxelBreakEffectManager::Load(void)
{
	for (VoxelBreakEffect*& voxel : voxels) {
		voxel = new VoxelBreakEffect(); 
		voxel->Load();
	}
}

void VoxelBreakEffectManager::Init(void)
{
	for (VoxelBreakEffect*& voxel : voxels) { voxel->Init(); }
}

void VoxelBreakEffectManager::Update(void)
{
	for (VoxelBreakEffect*& voxel : voxels) { voxel->Update(); }
}

void VoxelBreakEffectManager::Draw(void)
{
	for (VoxelBreakEffect*& voxel : voxels) { voxel->Draw(); }
}

void VoxelBreakEffectManager::Release(void)
{
	for (VoxelBreakEffect*& voxel : voxels) {
		if (!voxel) { continue; }
		voxel->Release();
		delete voxel;
		voxel = nullptr;
	}
}
