#pragma once

#include"Common/Transform.h"

#include"Common/Collider/ColliderBase.h"

#include"../Manager/Collision/CollisionManager.h"

class ActorBase
{
public:
	ActorBase();
	virtual ~ActorBase() = 0;

	virtual void Load(void) = 0;
	virtual void Init(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void Release(void);

	// モデルを複製する
	void ModelDuplicate(int model) { trans_.model = MV1DuplicateModel(model); }

	// モデル制御情報構造体のゲット関数
	Transform GetTrans(void)const { return trans_; }

	// 当たり判定の通知
	virtual void OnCollision(const ColliderBase& collider) {}

	// 接地判定の通知
	virtual void OnGrounded() {}

	/// <summary>
	/// コライダーすべてを取得
	/// </summary>
	std::vector<ColliderBase*> GetCollider(void)const { return collider_; }

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

	// 描画判定 （1 = 「描画する」、0 = 「描画しない」）
	unsigned char isDraw;

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

	// 当たり判定情報を生成
	void ColliderCreate(ColliderBase* newClass) {
		collider_.emplace_back(newClass);
		collider_.back()->SetTransformPtr(&trans_);
		collider_.back()->SetDynamicFlg((dynamicFlg_ == 1) ? true : false);
		collider_.back()->SetOnCollisionFunc([this](const ColliderBase& collider) { this->OnCollision(collider); });
		collider_.back()->SetOnGroundedFunc([this](void) {this->OnGrounded(); });
	}

	/// <summary>
	/// 特定のコライダーを探す
	/// </summary>
	/// <typeparam name="T">探したいクラス</typeparam>
	/// <param name="tag">タグ種類</param>
	/// <returns></returns>
	template<typename T>
	std::vector<T*> ColliderSerch(TAG tag = TAG::NON) {
		std::vector<T*> out;
		out.reserve(collider_.size());

		for (auto c : collider_) {
			if (!obj) continue;
			if (auto* ptr = dynamic_cast<T*>(c)) {
				if (c->GetTag() == tag || tag == TAG::NON) { out.push_back(ptr); }
			}
		}
		return out;
	}

	/// <summary>
	/// 移動するかを切り替える
	/// </summary>
	/// <param name="flg">1 = 「移動する」に切り替える、0 = 「移動しない」に切り替える</param>
	void SetDynamicFlg(bool flg) { 
		dynamicFlg_ = (flg) ? 1 : 0; 
		for (ColliderBase*& collider : GetCollider()) {
			collider->SetDynamicFlg(flg);
		}
	}

	/// <summary>
	/// 重力を適用するかを切り替える
	/// </summary>
	/// <param name="flg">1 = 「する」に切り替える、0 = 「しない」に切り替える</param>
	void SetGravityFlg(unsigned char flg) { isGravity = (flg == 0 || flg == 1) ? flg : isGravity; }

	/// <summary>
	/// 当たり判定フラグの取得
	/// </summary>
	/// <returns>どれか一つでも「判定する」状態ならtrue</returns>
	bool GetJudgeFlg(void) {
		for (const ColliderBase*& c : collider_) {
			if (!c) { continue; }
			if (c->GetJudge() == 1) { return true; }
		}
		return false;
	}

	// 当たり判定の設定（true = 「判定する」、false = 「判定しない」）
	void SetJudge(bool flg) {
		for (ColliderBase*& c : collider_) {
			if (!c) { continue; }
			c->SetJudgeFlg(flg);
		}
	}

	// 描画判定の取得
	bool GetIsDraw(void)const { return (isDraw == 1) ? true : false; }

	// 描画判定の設定（true = 「描画する」、false = 「描画しない」）
	void SetIsDraw(bool flg) { isDraw = (flg) ? 1 : 0; }

	// 派生先追加初期化
	virtual void SubInit(void) = 0;
	// 派生先追加更新
	virtual void SubUpdate(void) = 0;
	// 派生先追加描画
	virtual void SubDraw(void) = 0;
	// 派生先追加解放
	virtual void SubRelease(void) = 0;
};