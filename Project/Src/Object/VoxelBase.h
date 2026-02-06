#pragma once

#include"ActorBase.h"
#include<array>
#include<map>

#include"Common/Collider/VoxelCollider.h"

#include"Common/Effect/VoxelBreakEffect/VoxelBreakEffectManager.h"

class Camera;

class VoxelBase : public ActorBase
{
public:

	VoxelBase();
	virtual ~VoxelBase()override {};

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void AlphaDraw(void)override;
	void Release(void)override;

	struct MeshBatch {
		// 頂点配列
		std::vector<VERTEX3D> v;

		// インデックス配列
		std::vector<unsigned short> i;

		// 最小座標
		Vector3 bmin{ 1e9f, 1e9f, 1e9f };

		// 最大座標
		Vector3 bmax{ -1e9f,-1e9f,-1e9f };
	};

	// 全セルを復活させる
	void ReVival(void);

	// メッシュ群をワールド座標仕様で取得
	std::vector<MeshBatch> GetBatches(void)const {
		std::vector<MeshBatch>ret = {};
		
		ret = batches;

		for (auto& r : ret) {
			for (auto& vPos : r.v) { vPos.pos = (Vector3(vPos.pos) + trans_.pos).ToVECTOR(); }
		}

		return ret; 
	}

private:

#pragma region ユーティリティ
	int Idx(int x, int y, int z, int Nx, int Ny)const { return (z * Ny + y) * Nx + x; }
	int Idx(int x, int y, int z)const { return Idx(x, y, z, Nx, Ny); }

	Vector3 IdxReverse(int idx, int Nx, int Ny)const {
		Vector3 out = {};
		out.x = (float)(idx % Nx);
		int work = (idx - (int)out.x) / Nx;
		out.y = (float)(work % Ny);
		out.z = (work - out.y) / Ny;
		return out;
	}
	Vector3 IdxReverse(int idx)const { return IdxReverse(idx, Nx, Ny); }

	bool Inb(int x, int y, int z, int Nx, int Ny, int Nz)const { return 0 <= x && x < Nx && 0 <= y && y < Ny && 0 <= z && z < Nz; }
	bool Inb(int x, int y, int z)const { return Inb(x, y, z, Nx, Ny, Nz); }

	// セルインデックスからローカル座標を算出する
	Vector3 IdxToLocalPos(int idx)const {
		Vector3 p = IdxReverse(idx);
		return Vector3(
			(p.x - Nx / 2) * cellSize + (cellSize * 0.5f),
			(p.y - Ny / 2) * cellSize + (cellSize * 0.5f),
			(p.z - Nz / 2) * cellSize + (cellSize * 0.5f)
		);
	}

	// セルインデックスから座標を算出する
	Vector3 IdxToPos(int idx)const { return (trans_.pos + IdxToLocalPos(idx)); }

	// セル番地からローカル座標を算出する
	Vector3 IdxToLocalPos(int x, int y, int z)const {
		return Vector3(
			(x - Nx / 2) * cellSize + (cellSize * 0.5f),
			(y - Ny / 2) * cellSize + (cellSize * 0.5f),
			(z - Nz / 2) * cellSize + (cellSize * 0.5f)
		);
	}

	// セル番地から座標を算出する
	Vector3 IdxToPos(int x, int y, int z)const { return (trans_.pos + IdxToLocalPos(x, y, z)); }
#pragma endregion

	// 定数定義

	const Vector3 kDirNrm[6] = { {+1,0,0},{-1,0,0},{0,+1,0},{0,-1,0},{0,0,+1},{0,0,-1} };

#pragma region ボクセルメッシュを構成するメンバ変数
	// 密度情報
	std::vector<unsigned char> density;
	// 初期密度情報(破壊されたあと復活させるとき用)
	std::vector<unsigned char> densityInit;

	// グリッド数
	int Nx, Ny, Nz;

	// 大まかに全体を囲めるサイズ
	Vector3 roughSize;

	// セルサイズ
	float cellSize;

	// セル中心位置群(ローカル/ワールド)
	std::map<int,Vector3>cellCenterLocalPoss;
	std::map<int,Vector3>cellCenterPoss;

	// 再生成フラグ(壊されて形状が変化した時など)
	bool regeneration;

	// 生存に必要な密度比率(density_が１以上で生存扱い)
	float aliveNeedRatio;

	// メッシュバッチ群
	std::vector<MeshBatch> batches;

	// グリッド中心位置(モデルによる中心座標のズレの補完用)
	Vector3 gridCenter;

	// テクスチャID
	int texture;

	// その瞬間破壊されたセルのインデックスを格納する配列
	std::vector<int>breakCellIdx;
#pragma endregion

	// breakCellIdx(その瞬間破壊されたセルのインデックスを格納する配列)の情報を破棄＋最大配列数を生存セル数で固定
	void BreakCellIdxClear(void) { breakCellIdx.clear(); breakCellIdx.reserve(cellCenterPoss.size()); }

#pragma region メッシュ生成
	// 初期化時～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～
																			 
	/// <summary>
	/// MV1モデルからボクセルメッシュを生成
	/// </summary>
	/// <param name="mv1">（in）MV1モデルハンドル</param>
	/// <param name="cell">（in）セルサイズ</param>
	/// <param name="center">（in）ボクセルメッシュの中心座標</param>
	/// <param name="roughSize">（in）全体を囲める大まかなサイズ</param>
	/// <param name="batches">（out）生成されたメッシュ情報を格納するため変数</param>
	void BuildVoxelMeshFromMV1Handle(
		int mv1,
		float cell,
		const Vector3& center,
		const Vector3& roughSize,
		std::vector<MeshBatch>& batches
	);

	/// <summary>
	/// メッシュとの衝突判定で表面をマーキング
	/// </summary>
	/// <param name="mv1">（in）MV1モデルハンドル</param>
	/// <param name="cell">（in）セルサイズ</param>
	/// <param name="center">（in）ボクセルメッシュの中心座標</param>
	/// <param name="roughSize">（in）全体を囲める大まかなサイズ</param>
	/// <param name="Nx">（in）X方向のセル数</param>
	/// <param name="Ny">（in）Y方向のセル数</param>
	/// <param name="Nz">（in）Z方向のセル数</param>
	/// <param name="density">（out）密度情報格納用変数（ここでは表面のみ特定の数値を立てるだけ）</param>
	void MarkSurface(
		int mv1,
		float cell,
		const Vector3& center,
		const Vector3& roughSize,
		int Nx, int Ny, int Nz,
		std::vector<unsigned char>& density
	);
																			 

	/// <summary>
	/// MarkSurface()でマークした表面情報をもとにその中身を埋め、表面マーク用の数値を初期化
	/// </summary>
	/// <param name="density">（out）密度情報格納用変数（ここで初期密度情報は完成）</param>
	/// <param name="Nx">（in）X方向のセル数</param>
	/// <param name="Ny">（in）Y方向のセル数</param>
	/// <param name="Nz">（in）Z方向のセル数</param>
	void SolidFill(std::vector<unsigned char>& density, int Nx, int Ny, int Nz);
																			 
	// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

	/// <summary>
	/// density （密度情報）をもとに、メッシュを生成（グリーディー方式）
	/// </summary>
	/// <param name="density">（in）密度情報</param>
	/// <param name="Nx">（in）X方向のセル数</param>
	/// <param name="Ny">（in）Y方向のセル数</param>
	/// <param name="Nz">（in）Z方向のセル数</param>
	/// <param name="cell">（in）セルサイズ</param>
	/// <param name="batches">（out）生成されたメッシュ情報を格納するため変数</param>
	void BuildGreedyMesh(
		const std::vector<unsigned char>& density,
		int Nx, int Ny, int Nz,
		float cell, 
		std::vector<MeshBatch>& batches
	);

	float TEX_SCALE;

#pragma endregion

protected:

	// 派生先読み込み
	virtual void SubLoad(void) = 0;

	/// <summary>
	/// ボクセルメッシュ生成情報設定
	/// </summary>
	/// <param name="roughSize">大まかに全体を囲めるサイズ</param>
	/// <param name="texturePath">テクスチャId（すでに読み込まれているものを代入）（「指定なし」または「-１」で頂点カラーによる描画）</param>
	/// <param name="textureSize">テクスチャサイズ</param>
	/// <param name="cellSize">セルサイズ（標準は２０）</param>
	/// <param name="gridCenter">グリッド中心位置（モデルによる中心座標のズレの補完用）（標準は全て０）</param>
	/// <param name="aliveNeedRatio">生存に必要な密度比率（density_が１以上で生存扱い）（標準は１０％以上で生存）</param>
	void VoxelInfoInit(TAG colliderTag, const Vector3& roughSize, int texture = -1, float textureSize = 512.0f, float cellSize = 20.0f, const Vector3& gridCenter = Vector3(), float aliveNeedRatio = 0.1f) {
		this->roughSize = roughSize;
		this->texture = texture;
		this->TEX_SCALE = 1.0f / textureSize;
		this->cellSize = cellSize;
		this->gridCenter = gridCenter;
		this->aliveNeedRatio = aliveNeedRatio;
		// ボクセルオブジェクト専用のコライダーを生成
		ColliderCreate(new VoxelCollider(colliderTag, this->roughSize, this->cellSize, cellCenterPoss, (this->roughSize / 2).Length()));
	}

	/// <summary>
	/// ボクセルメッシュ生成情報設定
	/// </summary>
	/// <param name="roughSize">大まかに全体を囲めるサイズ</param>
	/// <param name="texturePath">テクスチャパス（「指定なし」または「""」で頂点カラーによる描画）</param>
	/// <param name="textureSize">テクスチャサイズ</param>
	/// <param name="cellSize">セルサイズ（標準は２０）</param>
	/// <param name="gridCenter">グリッド中心位置（モデルによる中心座標のズレの補完用）（標準は全て０）</param>
	/// <param name="aliveNeedRatio">生存に必要な密度比率（density_が１以上で生存扱い）（標準は１０％以上で生存）</param>
	void VoxelInfoInit(TAG colliderTag, const Vector3& roughSize, std::string texturePath = "", float textureSize = 512.0f, float cellSize = 20.0f, const Vector3& gridCenter = Vector3(), float aliveNeedRatio = 0.1f) {
		VoxelInfoInit(colliderTag, roughSize, (texturePath != "") ? LoadGraph(texturePath.c_str()) : -1, textureSize, cellSize, gridCenter, aliveNeedRatio);
	}

#pragma region 削る
	// 衝突情報群を取得して、該当セルの密度を引数指定分減少させる
	void ApplyBrush(unsigned char amount);

	// 破壊セルのインデックス群を取得
	const std::vector<int>& BreakCellIdx(void)const { return breakCellIdx; }
#pragma endregion

	// エフェクト
	VoxelBreakEffectManager* effect;

	void BreakEffect(const Vector3& breakerPos);
};

using MeshBatch = VoxelBase::MeshBatch;