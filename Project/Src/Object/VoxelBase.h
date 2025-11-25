#pragma once

#include"ActorBase.h"
#include<array>
#include<map>

#include"Common/Collider/VoxelCollider.h"

class Camera;

class VoxelBase : public ActorBase
{
public:
	const Vector3 kDirNrm[6] = { {+1,0,0},{-1,0,0},{0,+1,0},{0,-1,0},{0,0,+1},{0,0,-1} };

	VoxelBase();
	virtual ~VoxelBase()override {};

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void SetCamera(Camera* c) { camera_ = c; }

	struct MeshBatch {
		// 頂点配列
		std::vector<VERTEX3D> v;

		// インデックス配列
		std::vector<unsigned short> i;

		std::array<std::vector<unsigned short>, 6> iDir;

		// 最小座標
		Vector3 bmin{ 1e9f, 1e9f, 1e9f };

		// 最大座標
		Vector3 bmax{ -1e9f,-1e9f,-1e9f };

		// ローカル中心座標
		Vector3 centerLocal{ 0,0,0 };
	};

	// メッシュバッチ群を取得
	std::vector<MeshBatch> GetBatches(void)const { return batches_; }

	// 生存しているセルの中心座標を配列で取得
	std::map<int,Vector3> GetCellCenterPoss(void) const { return cellCenterPoss_; };
	std::map<int,Vector3> GetCellCenterWorldPoss(void) const { return cellCenterWorldPoss_; };

	// セルサイズを取得(float版)
	float GetCellSize(void)const { return cell_; }
	// セルサイズを取得(VECTOR版)
	Vector3 GetCellSizeVECTOR(void) const { return Vector3(cell_, cell_, cell_); }

	// 全セルを復活させる
	void ReVival(void);

#pragma region ユーティリティ
	int Idx(int x, int y, int z, int Nx, int Ny)const { return (z * Ny + y) * Nx + x; }
	int Idx(int x, int y, int z)const { return Idx(x, y, z, Nx_, Ny_); }

	Vector3 IdxReverse(int idx, int Nx, int Ny) const {
		Vector3 out;
		int layer = Nx * Ny;
		out.z = idx / layer;
		int rem = idx % layer;
		out.y = rem / Nx;
		out.x = rem % Nx;
		return out;
	}
	Vector3 IdxReverse(int idx) const { return IdxReverse(idx, Nx_, Ny_); }

	bool Inb(int x, int y, int z, int Nx, int Ny, int Nz) { return 0 <= x && x < Nx && 0 <= y && y < Ny && 0 <= z && z < Nz; }
	bool Inb(int x, int y, int z) { return Inb(x, y, z, Nx_, Ny_, Nz_); }
#pragma endregion

private:
#pragma region ボクセルメッシュを構成するメンバ変数
	// 密度情報
	std::vector<uint8_t> density_;
	// 初期密度情報(破壊されたあと復活させるとき用)
	std::vector<uint8_t> densityInit_;

	// グリッド数
	int Nx_, Ny_, Nz_;

	// 大まかに全体を囲めるサイズ
	Vector3 roughSize_;

	// セルサイズ
	float cell_;

	// セル中心位置群
	std::map<int,Vector3>cellCenterPoss_;
	std::map<int,Vector3>cellCenterWorldPoss_;

	// 再生成フラグ(壊されて形状が変化した時など)
	bool regeneration_;

	// 生存に必要な密度比率(density_が１以上で生存扱い)
	float aliveNeedRatio_;

	// メッシュバッチ群
	std::vector<MeshBatch> batches_;

	// グリッド中心位置(モデルによる中心座標のズレの補完用)
	Vector3 gridCenter_;

	// マーキング情報
	int marked;
	// ソリッドフィルフラグ
	int solid;
#pragma endregion

	// テクスチャID
	int textureId_;

	// カメラポインタ
	Camera* camera_;

#pragma region メッシュ生成
	// 初期化時～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～
																			 
	// MV1 ハンドルからボクセルメッシュを生成								 
	bool BuildVoxelMeshFromMV1Handle(
		int mv1,
		float cell,
		const Vector3& center,
		const Vector3& roughSize,
		std::vector<MeshBatch>& batches);

	// 衝突プローブで表面をマーキング										 
	void MarkSurfaceByCollisionProbe(
		int mv1,
		float cell, const Vector3& center, const Vector3& roughSize,
		int Nx, int Ny, int Nz, std::vector<uint8_t>& density);
																			 
	// 内部をソリッドにする												     
	void SolidFill(std::vector<uint8_t>& density, int Nx, int Ny, int Nz);
																			 
	// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

	// density からグリーディメッシュを生成
	void BuildGreedyMesh(const std::vector<uint8_t>& density, int Nx, int Ny, int Nz, float cell, std::vector<MeshBatch>& batches);
#pragma endregion

protected:

	virtual void SubLoad(void) = 0;

	/// <summary>
	/// ボクセルメッシュ生成情報設定
	/// </summary>
	/// <param name="roughSize">大まかに全体を囲めるサイズ</param>
	/// <param name="texturePath">テクスチャパス</param>
	/// <param name="cell">セルサイズ</param>
	/// <param name="gridCenter">グリッド中心位置(モデルによる中心座標のズレの補完用)</param>
	/// <param name="aliveNeedRatio">生存に必要な密度比率(density_が１以上で生存扱い)</param>
	void VoxelInfoInit(const Vector3& roughSize, std::string texturePath = "", float cell = 20.0f, const Vector3& gridCenter = Vector3(), float aliveNeedRatio = 0.1f) {
		roughSize_ = roughSize;
		textureId_ = (texturePath == "") ? LoadGraph(texturePath.c_str()) : -1;
		cell_ = cell;
		gridCenter_ = gridCenter;
		aliveNeedRatio_ = aliveNeedRatio;
	}


#pragma region 削る
	// 振り分け
	void ApplyBrush(unsigned char amount);
	
	bool nowFrameRemesh_;
#pragma endregion
};

using MeshBatch = VoxelBase::MeshBatch;