#pragma once

#include"../../../ActorBase.h"

class ThrowObjBase : public ActorBase
{
public:
	ThrowObjBase(const Vector3& playerPos_, const Vector3& playerAngle_);
	virtual ~ThrowObjBase() = default;

	void OnCollision(const ColliderBase& collider)override;

	enum class STATE
	{
		NON,
		CARRY,
		DROP,
		THROW,

		MAX
	};

	void Carry(void) { state = STATE::CARRY; }
	void Drop(void) { state = STATE::DROP; }
	virtual void Throw(void);

	STATE GetState(void)const { return state; }

private:

	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override {}
	void SubAlphaDraw(void)override {}
	void SubRelease(void)override {}

protected:
	// モデルハンドル
	int model;

	// 状態
	STATE state;

	// 関数ポインタ定義用
	using STATEFUNC = void (ThrowObjBase::*)(void);
	// 状態別関数ポインタ配列
	STATEFUNC stateFuncPtr[(int)STATE::MAX];

	// 状態別関数～～～～
	void NonStateFunc(void) {};
	void CarryStateFunc(void);
	void DropStateFunc(void);
	void ThrowStateFunc(void);
	//～～～～～～～～～


	// 投げた後の処理に使う～～～～～～

	// スピード(派生先で設定)
	float speed;
	// 移動ベクトル
	Vector3 moveVec;

	// 生存時間(派生先で設定)
	int aliveTime;
	// 生存時間計測用カウンター
	int aliveCounter;

	// 生存ヒット回数(派生先で設定)
	int ALIVE_HIT_NUM;
	// 生存ヒット回数計測用カウンター
	int aliveHitCou;
	//～～～～～～～～～～～～～～～～

	// プレイヤーが持ってるときの座標(プレイヤー座標からの相対座標)
	const Vector3 CARRY_OBJ_LOCAL_POS = { 100.0f,80.0f,0.0f };

	// プレイヤーが投げたときの初期座標(プレイヤー座標からの相対座標)
	const Vector3 LOCAL_THROW_POS = { 0.0f,50.0f,100.0f };
	// プレイヤーが投げたときの回転0のときの単位ベクトル
	const Vector3 THROW_VEC = { 0.0f,0.0f,1.0f };

	// プレイヤー座標(参照用)
	const Vector3& playerPos;
	// プレイヤー角度(参照用)
	const Vector3& playerAngle;
};