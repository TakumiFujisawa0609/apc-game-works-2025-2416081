#pragma once

#include"Common/Transform.h"

#include"Common/Collider/ColliderBase.h"

class ActorBase
{
public:
	ActorBase();
	virtual ~ActorBase() = 0;

	virtual void Load(void) = 0;
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	// モデル制御情報構造体のゲット関数
	Transform GetTrans(void)const { return trans_; }

	// 当たり判定の通知
	virtual void OnCollision(TAG type) {}

private:
	// 当たり判定情報
	std::vector<ColliderBase*> collider_;

	// 動的オブジェクトか否か（1 = 動的、0 = 静的）
	unsigned char dynamicFlg_;

	// 重力を適用するかどうか(1 = する、0 = しない)
	unsigned char isGravity;

	// １フレーム前の座標
	Vector3 prevPos_;

	// 加速度の更新
	void AccelUpdate(void);

	// 重力更新
	void Gravity(void);

	// 重力
	const float GRAVITY = -0.98f;
	const float GRAVITY_MAX = -30.0f;

	// 接地判定 管理用(派生先で変更不可で参照渡し)
	bool isGroundMaster;

protected:
	// モデル制御情報構造体
	Transform trans_;

	// 加速度
	VECTOR AccelSum;

	// 横軸加速度の１フレームごとの減衰量
	float ATTENUATION = 3.0f;
	// 横軸加速度の最大値
	float ACCEL_MAX = 30.0f;

	// 接地判定(派生先で参照用)
	const bool& isGround = isGroundMaster;

	// 当たり判定情報を設定
	void ColliderCreate(ColliderBase* newClass) {
		collider_.emplace_back(newClass);
		collider_.back()->SetTransformPtr(&trans_);
		collider_.back()->SetOnCollisionFun([this](TAG type) { this->OnCollision(type); });
	}

	/// <summary>
	/// 移動量加算を行うかを切り替える
	/// </summary>
	/// <param name="flg">1 = 「移動する」に切り替える、0 = 「移動しない」に切り替える</param>
	void SetDynamicFlg(unsigned char flg) { dynamicFlg_ = (flg == 0 || flg == 1) ? flg : dynamicFlg_; }

	/// <summary>
	/// 重力を適用するかを切り替える
	/// </summary>
	/// <param name="flg">1 = 「する」に切り替える、0 = 「しない」に切り替える</param>
	void SetGravityFlg(unsigned char flg) { isGravity = (flg == 0 || flg == 1) ? flg : isGravity; }

	// 派生先追加初期化
	virtual void SubInit(void) = 0;
	// 派生先追加更新
	virtual void SubUpdate(void) = 0;
	// 派生先追加描画
	virtual void SubDraw(void) = 0;
	// 派生先追加解放
	virtual void SubRelease(void) = 0;
};