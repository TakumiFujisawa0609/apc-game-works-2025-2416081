#pragma once

#include"../../../ActorBase.h"

class ThrowObjBase : public ActorBase
{
public:
	ThrowObjBase(const int& playerModel, const Vector3& playerAngle);
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

	void Carry(void) { state_ = STATE::CARRY; }
	void Drop(void) { state_ = STATE::DROP; }
	virtual void Throw(void);

	STATE GetState(void)const { return state_; }

private:
	const int PLAYER_RIGHTHAND_FRAME_INDEX = 14;
	const int& playerModel;
	const Vector3& playerAngle;

	const Vector3& PlayerRightHandPos(void)const { return Vector3(MV1GetFramePosition(playerModel, PLAYER_RIGHTHAND_FRAME_INDEX)); }

	void SubInit(void)override;
	void SubUpdate(void)override;

protected:
	int model;

	STATE state_;

	// ä÷êîÉ|ÉCÉìÉ^îzóÒ
	using STATEFUNC = void (ThrowObjBase::*)(void);
	STATEFUNC stateFuncPtr[(int)STATE::MAX];

	// èÛë‘ï ä÷êîÅ`Å`Å`Å`
	void NonStateFunc(void) {};
	void CarryStateFunc(void);
	void DropStateFunc(void);
	void ThrowStateFunc(void);
	//Å`Å`Å`Å`Å`Å`Å`Å`Å`


	// ìäÇ∞ÇΩå„ÇÃèàóùÇ…égÇ§Å`Å`Å`Å`Å`Å`
	Vector3 moveVec;

	float speed;

	int aliveTime;
	int aliveCounter;

	int ALIVE_HIT_NUM;
	int aliveHitCou;
	//Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

	const Vector3 THROW_VEC = { 0.0f,0.0f,1.0f };

};