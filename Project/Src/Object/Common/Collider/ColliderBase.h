#pragma once

#include <functional>

#include"../Transform.h"

class ColliderBase
{
public:

	// タイプ列挙型定義
	enum class TAG
	{
		NON = -1,
		PLAYER,
		ENEMY,
		STAGE,
	};

	// 形状列挙型定義
	enum class SHAPE
	{
		NON = -1,
		LINE,
		SPHERE,
		CAPSULE,
		AABB,
		MODEL,
		VOXEL,
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="type">当たり判定タイプ</param>
	/// <param name="enoughDistance">判定スキップに十分な距離　-1.0fで未設定とし、距離による判定スキップを行わない（引数省略で-1.0f）</param>
	/// <param name="pos">相対座標（引数省略で{0.0f,0.0f,0.0f}）</param>
	ColliderBase(TAG type, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f });
	virtual ~ColliderBase() = 0;

	// デバッグ描画
	void DrawDebug(unsigned int color = 0xffffff);

#pragma region 初期設定
	// モデル制御情報セット
	void SetTransformPtr(const Transform* ptr) { trans_ = ptr; }

	// 当たり判定通知用関数セット
	void SetOnCollisionFun(std::function<void(TAG type)> OnCollisionFunc) { OnCollision = std::move(OnCollisionFunc); }
#pragma endregion

#pragma region 各ゲット関数
	// 座標
	Vector3 GetPos(void)const { return (trans_->WorldPos() + trans_->VTrans(pos_)); }

	// 判定スキップに十分な距離
	float GetEnoughDistance(void)const { return enoughDistance_; }

	// 当たり判定のタイプ
	TAG GetType(void)const { return type_; }

	// 当たり判定の形状
	SHAPE GetShape(void)const { return shape_; }

	// 判定通知の呼び出し
	void CallOnCollision(TAG type) { OnCollision(type); }
#pragma endregion

private:
	// モデル制御情報をポインタで受け取って保持
	const Transform* trans_;

	// モデル制御情報の座標からの相対座標
	Vector3 pos_;

	// 絶対に当たらない距離（判定時早期リターン用）
	float enoughDistance_;

	// 当たり判定タイプ（何と当たったかを見分ける用）
	TAG type_;

	// 当たり判定形状
	SHAPE shape_;
	
	// 当たったときに呼び出す関数をポインタで受け取って保持
	std::function<void(TAG type)>OnCollision;
};

using TAG = ColliderBase::TAG;
using SHAPE = ColliderBase::SHAPE;