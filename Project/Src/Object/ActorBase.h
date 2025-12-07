#pragma once

#include"Common/Transform.h"

#include"Common/Collider/ColliderBase.h"

#include"../Manager/Collision/CollisionManager.h"

class ActorBase
{
public:
	ActorBase();
	virtual ~ActorBase() = default;

	virtual void Load(void) = 0;
	virtual void Init(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void AlphaDraw(void);
	virtual void Release(void);

	// モデルを複製する
	void ModelDuplicate(int model) { trans_.model = MV1DuplicateModel(model); }

	// モデル制御情報構造体のゲット関数
	const Transform& GetTrans(void)const { return trans_; }

	// 当たり判定の通知
	virtual void OnCollision(const ColliderBase& collider) {}

	// 接地判定の通知
	virtual void OnGrounded() {}

	/// <summary>
	/// コライダーすべてを取得
	/// </summary>
	virtual std::vector<ColliderBase*> GetCollider(void)const { return collider_; }

	/// <summary>
	/// 当たり判定フラグの取得
	/// </summary>
	/// <returns>どれか一つでも「判定する」状態ならtrue</returns>
	bool GetJudgeFlg(void) {
		for (ColliderBase*& c : collider_) {
			if (!c) { continue; }
			if (c->GetJudge() == 1) { return true; }
		}
		return false;
	}

	// 描画判定の取得
	bool GetIsDraw(void)const { return (isDraw == 1) ? true : false; }

private:
	// 当たり判定情報
	std::vector<ColliderBase*> collider_;

	// 動的オブジェクトか否か（1 = 動的、0 = 静的）
	unsigned char dynamicFlg_;

	// 重力を適用するかどうか(1 = する、0 = しない)
	unsigned char isGravity_;

	// 衝突時押し出しを行うか否か（1 = 押し出す、0 = 押し出さず通り抜ける）
	unsigned char pushFlg_;

	// 押し出しを行う際の重さ（0 ～ 100 で設定）
	unsigned char pushWeight_;

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

	// アルファ判定（1 = 「メインの描画にアルファをかける」、0 = 「メインの描画を普通に描画する」）（メインの描画 = 基底クラスで自動で描画されるモデルなど）
	unsigned char isAlphaDraw;

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
			if (!c) continue;
			if (auto* ptr = dynamic_cast<T*>(c)) {
				if (c->GetTag() == tag || tag == TAG::NON) { out.push_back(ptr); }
			}
		}
		return out;
	}
#pragma region 初期設定
	// 当たり判定情報を生成
	void ColliderCreate(ColliderBase* newClass) {
		collider_.emplace_back(newClass);
		collider_.back()->SetTransformPtr(&trans_);
		collider_.back()->SetDynamicFlg((dynamicFlg_ == 1) ? true : false);
		collider_.back()->SetPushFlg(pushFlg_);
		collider_.back()->SetPushWeight(pushWeight_);
		collider_.back()->SetOnCollisionFunc([this](const ColliderBase& collider) { this->OnCollision(collider); });
		collider_.back()->SetOnGroundedFunc([this](void) {this->OnGrounded(); });
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
	/// <param name="flg">true = 「する」に切り替える、false = 「しない」に切り替える</param>
	void SetGravityFlg(bool flg) { isGravity_ = (flg) ? 1 : 0; }

	/// <summary>
	/// 衝突時押し出しを行うかを設定する
	/// </summary>
	/// <param name="flg">true = 押し出す、false = 押し出さず通り抜ける</param>
	void SetPushFlg(bool flg) {
		pushFlg_ = (flg) ? 1 : 0;
		for (ColliderBase*& coll : collider_) { coll->SetPushFlg(flg); }
	}

	/// <summary>
	/// 押し出しを行う際の重さ
	/// </summary>
	/// <param name="weight">0 ～ 100 で設定（数値が大きいほど重い）</param>
	void SetPushWeight(unsigned char weight) {
		pushWeight_ = weight;
		for (ColliderBase*& coll : collider_) { coll->SetPushWeight(weight); }
	}
#pragma endregion

	// 当たり判定の設定（true = 「判定する」、false = 「判定しない」）
	void SetJudge(bool flg) {
		for (ColliderBase*& c : collider_) {
			if (!c) { continue; }
			c->SetJudgeFlg(flg);
		}
	}

	// 描画判定の設定（true = 「描画する」、false = 「描画しない」）
	void SetIsDraw(bool flg) { isDraw = (flg) ? 1 : 0; }
	// 描画判定の設定（引数省略で現在の逆にスイッチ）
	void SetIsDraw(void) { isDraw = 1 - isDraw; }

	// アルファ判定の設定（true = 「メインの描画にアルファをかける」、false = 「メインの描画を普通に描画する」）（メインの描画 = 基底クラスで自動で描画されるモデルなど）
	void SetIsAlphaDraw(bool flg) { isAlphaDraw = (flg) ? 1 : 0; }
	// アルファ判定の設定（引数省略で現在の逆にスイッチ）
	void SetIsAlphaDraw(void) { isAlphaDraw = 1 - isAlphaDraw; }

	// 派生先追加初期化
	virtual void SubInit(void) = 0;
	// 派生先追加更新
	virtual void SubUpdate(void) = 0;
	// 派生先追加描画
	virtual void SubDraw(void) = 0;
	// 派生先追加アルファ描画
	virtual void SubAlphaDraw(void) = 0;
	// 派生先追加解放
	virtual void SubRelease(void) = 0;
};