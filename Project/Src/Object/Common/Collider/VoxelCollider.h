#pragma once

#include<map>

#include"ColliderBase.h"

class VoxelCollider : public ColliderBase
{
public:
	VoxelCollider(TAG type, const Vector3& roughSize, const float& cellSize, const std::map<int, Vector3>& cellWorldPoss, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }) :
		ColliderBase(type, enoughDistance, pos),
		roughSize_(roughSize),
		cellSize_(cellSize),
		cellWorldPoss_(cellWorldPoss)
	{
		SetShape(SHAPE::VOXEL);
	}
	~VoxelCollider()override {}

#pragma region 各ゲット関数

	// 全体を囲める大まかなサイズを取得
	const Vector3& GetRoughSize(void)const { return roughSize_; }
	
	// セルサイズを取得(float版)
	const float& GetCellSize(void)const { return cellSize_; }
	// セルサイズを取得(Vector3版)
	Vector3 GetCellSizeVECTOR(void) const { return Vector3(cellSize_, cellSize_, cellSize_); }

	// 生存しているセルのワールド中心座標群を取得
	std::map<int, Vector3> GetCellWorldPoss(void)const { return cellWorldPoss_; }

	// 当たり判定毎に更新してその衝突判定であたったセルのインデックスを格納する配列を取得
	const std::vector<int>& GetHitCellIdxs(void)const { return hitCellIdxs_; }

#pragma endregion

#pragma region 各セット関数

	// 当たり判定毎に更新してその衝突判定であたったセルのインデックスを格納する配列をクリア
	void ClearHitCellIdxs(void) { hitCellIdxs_.clear(); }
	// 当たり判定毎に更新してその衝突判定であたったセルのインデックスを格納する配列をセット
	void SetHitCellIdxs(const  std::vector<int>& idxs) { hitCellIdxs_.clear(); hitCellIdxs_ = idxs; }
	// 当たり判定毎に更新してその衝突判定であたったセルのインデックスを格納する配列に追加
	void AddHitCellIdx(int idx) { hitCellIdxs_.push_back(idx); }

#pragma endregion

	void DrawDebug(unsigned int color = 0xffffff)override {

		float cellSizeHalf = GetCellSize() * 0.5f;

		for (std::pair<const int, Vector3> cellPos : GetCellWorldPoss()) {
			DrawSphere3D(cellPos.second.ToVECTOR(), cellSizeHalf, 4, color, color, true);
		}

		Vector3 half = roughSize_ * 0.5f;
		Vector3 center = GetPos();

		// 8頂点
		Vector3 p[8] =
		{
			{ center.x - half.x, center.y - half.y, center.z - half.z }, // 0
			{ center.x + half.x, center.y - half.y, center.z - half.z }, // 1
			{ center.x + half.x, center.y + half.y, center.z - half.z }, // 2
			{ center.x - half.x, center.y + half.y, center.z - half.z }, // 3

			{ center.x - half.x, center.y - half.y, center.z + half.z }, // 4
			{ center.x + half.x, center.y - half.y, center.z + half.z }, // 5
			{ center.x + half.x, center.y + half.y, center.z + half.z }, // 6
			{ center.x - half.x, center.y + half.y, center.z + half.z }, // 7
		};

		// 辺を描く（12本）
		auto L = [&](int a, int b)
			{
				DrawLine3D(p[a].ToVECTOR(), p[b].ToVECTOR(), color);
			};

		// 手前
		L(0, 1); L(1, 2); L(2, 3); L(3, 0);
		// 奥
		L(4, 5); L(5, 6); L(6, 7); L(7, 4);
		// 側面
		L(0, 4); L(1, 5); L(2, 6); L(3, 7);
	}

private:
	// 全体を囲める大まかなサイズ
	Vector3 roughSize_;

	// セルサイズ
	const float& cellSize_;

	// 生存しているセルのローカル中心座標群
	const std::map<int, Vector3>& cellWorldPoss_;

	// 当たり判定毎に更新してその衝突判定であたったセルのインデックスを格納する配列
	std::vector<int> hitCellIdxs_;
};