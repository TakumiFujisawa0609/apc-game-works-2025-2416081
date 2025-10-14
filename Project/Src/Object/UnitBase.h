#pragma once

#include <DxLib.h>
#include<vector>
#include<map>

#include"../Utility/Utility.h"

enum class CollisionShape { NON = -1, SPHERE, OBB, AABB, CAPSULE, MAX };
enum class CollisionType { NON = -1, ALLY/*味方*/, ENEMY/*敵*/ };

struct Base
{
	struct {

		// 当たり判定ステータスーーーーーーーーーーーーーーーーーーーーーーーーーーー
		CollisionShape colliShape = CollisionShape::NON;	// 形状
		CollisionType colliType = CollisionType::NON;		// 属性

		VECTOR size = { -1.0f, -1.0f, -1.0f };				// OBB 形状の直径(各軸の長さ)
		float  radius = -1.0f;								// SPHERE 形状の半径 or CAPSULE 形状の半径
		float capsuleHalfLen = -1.0f;						// CAPSULE 形状の線分の長さ
		VECTOR center = { 0.0f, 0.0f, 0.0f };				// 原点からの中心オフセット
		// ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

		float  speed = -1.0f;	// スピード

	} para_;

	bool   isAlive_ = false;				// 生存フラグ
	bool   aliveCollision_ = true;			// 生存フラグによる当たり判定早期リターン

	VECTOR pos_ = { -1.0f, -1.0f, -1.0f };	// ワールド座標（中心）
	VECTOR prevPos_ = { -1.0f,-1.0f,-1.0f };
	VECTOR WorldPos(void)const { return VAdd(pos_, para_.center); }

	VECTOR vel_ = { 0.0f,0.0f,0.0f };

	VECTOR angle_ = { 0.0f,0.0f,0.0f };	// 角度

	int    hp_ = -1;						// ヒットポイント

	int    inviciCounter_ = 0;				// 無敵カウンター(０より大きいとき無敵)
	bool   isInvici_ = true;				// 上の無敵カウンターを当たり判定で適用するかどうか

	int model_ = -1;						// モデルハンドル番号を格納
};

class UnitBase
{
public:
	UnitBase();
	virtual ~UnitBase() = 0;

	virtual void Load(void) = 0;
	virtual void Init(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	virtual void UiDraw(void) {}
	virtual void Release(void) = 0;

	const Base& GetUnit(void) const { return unit_; }
	virtual int GetState(void)const { return -1; }

	void SetPos(const VECTOR& p) { unit_.pos_ = p; }
	void SetVelocity(const VECTOR& v) { unit_.vel_ = v; }

	virtual void OnGrounded() {}
	virtual void OnCollision(UnitBase* other) = 0;

	void BeginFrame() { unit_.prevPos_ = unit_.pos_; }
	void Integrate(void) { unit_.pos_ = VAdd(unit_.pos_, unit_.vel_); ZeroVelocityAxes(true, true, true); }
	void ZeroVelocityAxes(bool x, bool y, bool z) { if (x) { unit_.vel_.x = 0; } if (y) { unit_.vel_.y = 0; } if (z) { unit_.vel_.z = 0; } }

protected:
	Base unit_;
	void Invi(void);

	float yAccelSum_;

	void AccelUpdate(void);

#pragma region 重力処理（機能を置いとくだけ）
	static constexpr float GRAVITY = -0.8;
	static constexpr float GRAVITY_MAX = -40.0f;
	void Gravity(void);
#pragma endregion
};