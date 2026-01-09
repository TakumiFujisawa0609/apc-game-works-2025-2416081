#include"VoxelBreakEffectManager.h"

VoxelBreakEffectManager::VoxelBreakEffectManager(const int texture) :
	texture(texture),

	voxels()
{
}

void VoxelBreakEffectManager::Load(void)
{
	for (VoxelBreakEffect*& voxel : voxels) {
		voxel = new VoxelBreakEffect(texture); 
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

void VoxelBreakEffectManager::Spawn(const Vector3& pos, float cellSize, const Vector3& vel)
{
	for (VoxelBreakEffect*& voxel : voxels) {
		if (voxel->IsActive()) { continue; }

		voxel->Spawn(pos, cellSize, vel);

		break;
	}
}