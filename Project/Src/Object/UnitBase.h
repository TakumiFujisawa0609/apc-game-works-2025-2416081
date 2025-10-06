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
	virtual void Release(void) = 0;

	const Base& GetUnit(void) const { return unit_; }
	virtual int GetState(void)const { return -1; }
	virtual void OnCollision(UnitBase* other) = 0;

protected:
	Base unit_;
	void Invi(void);


	VECTOR prevPos_;

	float yAccelSum_;

	void AccelUpdate(void);

#pragma region 重力処理（機能を置いとくだけ）
	static constexpr float GRAVITY = -49.8f;
	static constexpr float GRAVITY_MAX = -60.0f;
	void Gravity(void);
#pragma endregion


};