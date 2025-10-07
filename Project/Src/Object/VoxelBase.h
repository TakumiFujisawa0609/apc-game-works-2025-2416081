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

	virtual void OnCollision(UnitBase* other)override {}

	struct MeshBatch {
		std::vector<VERTEX3D> v;
		std::vector<unsigned short> i;
		std::array<std::vector<unsigned short>, 6> iDir;
		VECTOR bmin{ 1e9f, 1e9f, 1e9f };
		VECTOR bmax{ -1e9f,-1e9f,-1e9f };
		VECTOR centerLocal{ 0,0,0 };

		// ワールド頂点キャッシュ
		std::vector<VERTEX3D> vWorld;
		VECTOR lastOff{ 1e30f,1e30f,1e30f };
	};

	std::vector<MeshBatch> GetBatches(void)const { return batches_; }

	bool ResolveCapsule(VECTOR& footPos, float R, float halfH, VECTOR& vel, bool& grounded, float slopeLimitDeg, int maxIters);



	struct AABB { VECTOR min; VECTOR max; };
	std::vector<VECTOR> GetVoxelCenters(void) const;
	std::vector<AABB> GetVoxelAABBs(void) const;

	float GetCellSize(void)const { return cell_; }
	VECTOR GetCellSizeVECTOR(void) const { return VGet(cell_, cell_, cell_); }

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

	float aliveNeedRatio_;



	int Idx(int x, int y, int z, int Nx, int Ny)const { return (z * Ny + y) * Nx + x; }
	int Idx(int x, int y, int z)const { return Idx(x, y, z, Nx_, Ny_); }
	bool Inb(int x, int y, int z, int Nx, int Ny, int Nz) { return 0 <= x && x < Nx && 0 <= y && y < Ny && 0 <= z && z < Nz; }
	bool Inb(int x, int y, int z) { return Inb(x, y, z, Nx_, Ny_, Nz_); }

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


	// 削る
	bool ApplyBrush(const Base& other, uint8_t amount);
	
	// 球体で削る
	bool ApplyBrushSphere(const Base& other, uint8_t amount);
	// AABB で削る
	bool ApplyBrushAABB(const Base& other, uint8_t amount);
	// カプセルで削る
	bool ApplyBrushCapsule(const Base& other, uint8_t amount);

};

using MeshBatch = VoxelBase::MeshBatch;