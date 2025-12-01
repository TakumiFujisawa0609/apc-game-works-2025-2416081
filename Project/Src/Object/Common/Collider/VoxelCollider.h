#pragma once

#include<map>

#include"ColliderBase.h"

class VoxelCollider : public ColliderBase
{
public:
	VoxelCollider(TAG type, const float& cellSize, const std::map<int, Vector3>& cellLocalPoss, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }) :
		ColliderBase(type, enoughDistance, pos),
		cellSize_(cellSize),
		cellLocalPoss_(cellLocalPoss)
	{
	}
	~VoxelCollider()override {}

#pragma region 各ゲット関数
	
	// セルサイズを取得(float版)
	const float& GetCellSize(void)const { return cellSize_; }
	// セルサイズを取得(Vector3版)
	const Vector3 GetCellSizeVECTOR(void) const { return Vector3(cellSize_, cellSize_, cellSize_); }

	// 生存しているセルのローカル中心座標群を取得
	const std::map<int, Vector3>& GetCellLocalPoss(void) const { return cellLocalPoss_; }
	// 生存しているセルのワールド中心座標群を取得
	std::map<int, Vector3> GetCellWorldPoss(void)const {
		std::map<int, Vector3> ret;
		for (const std::pair<int,Vector3>& localPos : cellLocalPoss_) {
			ret[localPos.first] = GetPos() + localPos.second;
		}
		return ret;
	}

	// 当たり判定毎に更新してその衝突判定であたったセルのインデックスを格納する配列を取得
	const std::vector<int>& GetHitCellIdxs(void)const { return hitCellIdxs_; }

#pragma endregion

#pragma region 各セット関数

	// 当たり判定毎に更新してその衝突判定であたったセルのインデックスを格納する配列をクリア
	void ClearHitCellIdxs(void) { hitCellIdxs_.clear(); }
	// 当たり判定毎に更新してその衝突判定であたったセルのインデックスを格納する配列をセット
	void SetHitCellIdxs(const  std::vector<int>& idxs) { hitCellIdxs_.clear(); hitCellIdxs_ = std::move(idxs); }
	// 当たり判定毎に更新してその衝突判定であたったセルのインデックスを格納する配列に追加
	void AddHitCellIdx(int idx) { hitCellIdxs_.push_back(idx); }

#pragma endregion


private:
	// セルサイズ
	const float& cellSize_;

	// 生存しているセルのローカル中心座標群
	const std::map<int, Vector3>& cellLocalPoss_;

	// 当たり判定毎に更新してその衝突判定であたったセルのインデックスを格納する配列
	std::vector<int> hitCellIdxs_;
};