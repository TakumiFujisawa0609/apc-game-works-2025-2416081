#pragma once

#include"UnitBase.h"
#include<array>

class Camera;

class VoxelBase : public UnitBase
{
public:
	static constexpr VECTOR kDirNrm[6] = { {+1,0,0},{-1,0,0},{0,+1,0},{0,-1,0},{0,0,+1},{0,0,-1} };

	VoxelBase();
	virtual ~VoxelBase()override {};

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void SetCamera(Camera* c) { camera_ = c; }

	virtual void OnCollision(UnitBase* other);

	struct MeshBatch {
		std::vector<VERTEX3D> v;
		std::vector<unsigned short> i;
		std::array<std::vector<unsigned short>, 6> iDir;
		VECTOR bmin{ 1e9f, 1e9f, 1e9f };
		VECTOR bmax{ -1e9f,-1e9f,-1e9f };
		VECTOR centerLocal{ 0,0,0 };
	};


protected:
	virtual void SubLoad(void) = 0;
	virtual void SubInit(void) = 0;
	virtual void SubUpdate(void) = 0;
	virtual void SubDraw(void) = 0;
	virtual void SubRelease(void) = 0;

	Camera* camera_;

	int textureId_;

	std::vector<MeshBatch> batches_;

	int marked, solid;

	int Nx_, Ny_, Nz_;
	float cell_;
	VECTOR gridCenter_;
	std::vector<uint8_t> density_;

	bool regeneration_;


	int Idx(int x, int y, int z, int Nx, int Ny) { return (z * Ny + y) * Nx + x; }
	int Idx(int x, int y, int z) { return Idx(x, y, z, Nx_, Ny_); }
	bool Inb(int x, int y, int z, int Nx, int Ny, int Nz) { return 0 <= x && x < Nx && 0 <= y && y < Ny && 0 <= z && z < Nz; }

	void SolidFill(std::vector<uint8_t>& density, int Nx, int Ny, int Nz);
	void MarkSurfaceByCollisionProbe(int mv1, float cell, const VECTOR& center, const VECTOR& halfExt,
		int Nx, int Ny, int Nz, std::vector<uint8_t>& density);
	void BuildCubicMesh(const std::vector<uint8_t>& density, int Nx, int Ny, int Nz,
		float cell, const VECTOR& center,
		std::vector<MeshBatch>& batches);
	bool BuildVoxelMeshFromMV1Handle(
		int mv1,
		float cell,
		const VECTOR& center,
		const VECTOR& halfExt,
		std::vector<MeshBatch>& batches);


	// çÌÇÈ
	void ApplyBrush(const Base& other, uint8_t amount);
	
	// ãÖëÃÇ≈çÌÇÈ
	void ApplyBrushSphere(const Base& other, uint8_t amount);
	// AABB Ç≈çÌÇÈ
	void ApplyBrushAABB(const Base& other, uint8_t amount);
	// ÉJÉvÉZÉãÇ≈çÌÇÈ
	void ApplyBrushCapsule(const Base& other, uint8_t amount);
};

using MeshBatch = VoxelBase::MeshBatch;