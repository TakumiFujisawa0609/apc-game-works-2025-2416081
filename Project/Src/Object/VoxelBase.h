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
		// 頂点配列
		std::vector<VERTEX3D> v;

		// インデックス配列
		std::vector<unsigned short> i;

		std::array<std::vector<unsigned short>, 6> iDir;

		// 最小座標
		VECTOR bmin{ 1e9f, 1e9f, 1e9f };

		// 最大座標
		VECTOR bmax{ -1e9f,-1e9f,-1e9f };

		// ローカル中心座標
		VECTOR centerLocal{ 0,0,0 };
	};

	// メッシュバッチ群を取得
	std::vector<MeshBatch> GetBatches(void)const { return batches_; }

	// 押し戻し処理（カプセル版）
	bool ResolveCapsule(VECTOR& center, float R, float halfH, VECTOR& vel, bool& grounded, float slopeLimitDeg, int maxIter);

	// 生存しているセルの中心座標を配列で取得
	std::vector<VECTOR> GetCellCenterPoss(void) const { return cellCenterPoss_; };

	// セルサイズを取得(float版)
	float GetCellSize(void)const { return cell_; }

	// セルサイズを取得(VECTOR版)
	VECTOR GetCellSizeVECTOR(void) const { return VGet(cell_, cell_, cell_); }

	// 全セルを復活させる
	void ReVival(void);

protected:
	virtual void SubLoad(void) = 0;
	virtual void SubInit(void) = 0;
	virtual void SubUpdate(void) = 0;
	virtual void SubDraw(void) = 0;
	virtual void SubRelease(void) = 0;

	Camera* camera_;

	// テクスチャID
	int textureId_;

#pragma region ボクセルメッシュを構成するメンバ変数
	// 密度情報
	std::vector<uint8_t> density_;
	// 初期密度情報(破壊されたあと復活させるとき用)
	std::vector<uint8_t> densityInit_;

	// グリッド数
	int Nx_, Ny_, Nz_;

	// セルサイズ
	float cell_;

	// セル中心位置群
	std::vector<VECTOR>cellCenterPoss_;

	// 再生成フラグ(壊されて形状が変化した時など)
	bool regeneration_;

	// 生存に必要な密度比率(density_が１以上で生存扱い)
	float aliveNeedRatio_;

	// メッシュバッチ群
	std::vector<MeshBatch> batches_;

	// グリッド中心位置(モデルによる中心座標のズレの補完用)
	VECTOR gridCenter_;

	// マーキング情報
	int marked;
	// ソリッドフィルフラグ
	int solid;
#pragma endregion

#pragma region ユーティリティ
	int Idx(int x, int y, int z, int Nx, int Ny)const { return (z * Ny + y) * Nx + x; }
	int Idx(int x, int y, int z)const { return Idx(x, y, z, Nx_, Ny_); }
	bool Inb(int x, int y, int z, int Nx, int Ny, int Nz) { return 0 <= x && x < Nx && 0 <= y && y < Ny && 0 <= z && z < Nz; }
	bool Inb(int x, int y, int z) { return Inb(x, y, z, Nx_, Ny_, Nz_); }
#pragma endregion

#pragma region メッシュ生成
	// 初期化時～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～
																			 
	// MV1 ハンドルからボクセルメッシュを生成								 
	bool BuildVoxelMeshFromMV1Handle(
		int mv1,
		float cell,
		const VECTOR& center,
		const VECTOR& halfExt,
		std::vector<MeshBatch>& batches);

	// 衝突プローブで表面をマーキング										 
	void MarkSurfaceByCollisionProbe(										 
		int mv1,															 
		float cell, const VECTOR& center, const VECTOR& halfExt,			 
		int Nx, int Ny, int Nz, std::vector<uint8_t>& density);				 
																			 
	// 内部をソリッドにする												     
	void SolidFill(std::vector<uint8_t>& density, int Nx, int Ny, int Nz);	 
																			 
	// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

	// density からグリーディメッシュを生成
	void BuildGreedyMesh(const std::vector<uint8_t>& density, int Nx, int Ny, int Nz, float cell, std::vector<MeshBatch>& batches);
#pragma endregion

#pragma region 削る
	// 振り分け
	bool ApplyBrush(const Base& other, uint8_t amount);
	
	// 球体
	bool ApplyBrushSphere(const Base& other, uint8_t amount);
	// AABB
	bool ApplyBrushAABB(const Base& other, uint8_t amount);
	// カプセル
	bool ApplyBrushCapsule(const Base& other, uint8_t amount);

	bool nowFrameRemesh_;
#pragma endregion
};

using MeshBatch = VoxelBase::MeshBatch;