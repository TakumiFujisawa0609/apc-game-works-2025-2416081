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
		PLAYER_PUNCH,
		PLAYER_GOUGE,
		PLAYER_THROWING,

		ENEMY,

		BOSS,
		GOLEM_ATTACK_FALL,
		GOLEM_ATTACK_PSYCHOROCK,
		GOLEM_ATTACK_STONE,
		
		STAGE,
	};

	// 形状列挙型定義
	enum class SHAPE
	{
		NON = -1,
		LINE,
		SPHERE,
		CAPSULE,
		BOX,
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
	void SetTransformPtr(Transform* ptr) { trans_ = ptr; }

	// 当たり判定通知用関数セット
	void SetOnCollisionFun(std::function<void(const ColliderBase& type)> OnCollisionFunc) { OnCollision = std::move(OnCollisionFunc); }
#pragma endregion

#pragma region 各ゲット関数
	// コライダー座標（モデル制御情報の座標がそのままコライダーの座標とは限らない為、計算済みの座標を取得する関数を用意）
	Vector3 GetPos(void)const { return (trans_->WorldPos() + trans_->VTrans(pos_)); }

	// 1フレーム前のコライダー座標（モデル制御情報の座標がそのままコライダーの座標とは限らない為、計算済みの座標を取得する関数を用意）
	Vector3 GetPrevPos(void)const { return (trans_->WorldPrevPos() + trans_->VTrans(pos_)); }

	// モデル制御情報を直接取得
	const Transform& GetTransform(void)const { return *trans_; }

	// 動的オブジェクトか否か（true = 動的、false = 静的）
	bool GetDynamicFlg(void)const { return (dynamicFlg_ == 1) ? true : false; }

	// 判定スキップに十分な距離
	float GetEnoughDistance(void)const { return enoughDistance_; }

	// 当たり判定フラグ（true = 「判定する」、false = 「判定しない」）
	bool GetJudge(void)const { return (judgeFlg_ == 1) ? true : false; }

	// 押し出しのフラグ
	bool GetPushFlg(void)const { return (pushFlg_ == 1) ? true : false; }

	// 押し出しを行う際の重さ（0 ～ 100）
	unsigned char GetPushWeight(void)const { return pushWeight_; }

	// 当たり判定のタイプ
	TAG GetTag(void)const { return type_; }

	// 当たり判定の形状
	SHAPE GetShape(void)const { return shape_; }

	// 判定通知の呼び出し
	void CallOnCollision(const ColliderBase& collider) { OnCollision(collider); }
#pragma endregion

#pragma region 各セット関数
	// モデル制御情報の座標情報を書き換える
	void SetTransformPos(const Vector3& pos) { trans_->pos = pos; }

	// 動的オブジェクトか否かを設定する（true = 動的、false = 静的）
	void SetDynamicFlg(bool flg) { dynamicFlg_ = (flg) ? 1 : 0; }

	// 当たり判定フラグセット（true = 「判定する」、false = 「判定しない」）
	void SetJudgeFlg(bool flg) { judgeFlg_ = (flg) ? 1 : 0; }

	// 押し出しを行うかどうかのフラグを設定（true = 「押し出す」、false = 「押し出さない」）
	void SetPushFlg(bool flg) { pushFlg_ = (flg) ? 1 : 0; }
#pragma endregion


private:
	// モデル制御情報をポインタで受け取って保持
	Transform* trans_;
	
	// モデル制御情報の座標からの相対座標
	Vector3 pos_;

	// 絶対に当たらない距離（判定時早期リターン用）
	float enoughDistance_;

	// 動的オブジェクトか否か（1 = 動的、0 = 静的）
	unsigned char dynamicFlg_;

	// 当たり判定フラグ（1 = 「判定する」、0 = 「判定しない」）
	unsigned char judgeFlg_;

	// 判定をする場合、押し出しを行うかどうかのフラグ（1 = 「押し出す」、0 = 「押し出さず通り抜ける」）
	unsigned char pushFlg_;

	// 押し出しを行う際の重さ（0 ～ 100 で設定）
	unsigned char pushWeight_;
	

	// 当たり判定タイプ（何と当たったかを見分ける用）
	TAG type_;

	// 当たり判定形状
	SHAPE shape_;
	
	// 当たったときに呼び出す関数をポインタで受け取って保持
	std::function<void(const ColliderBase& type)>OnCollision;
};

using TAG = ColliderBase::TAG;
using SHAPE = ColliderBase::SHAPE;