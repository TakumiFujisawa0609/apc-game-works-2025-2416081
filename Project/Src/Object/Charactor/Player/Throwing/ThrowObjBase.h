#pragma once

#include"../../../ActorBase.h"

class ThrowObjBase : public ActorBase
{
public:
	ThrowObjBase(const Vector3& playerPos_, const Vector3& playerAngle_);
	virtual ~ThrowObjBase() = default;

	void Load(void)override;

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

	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override {}
	void SubAlphaDraw(void)override {}
	void SubRelease(void)override {}

protected:
	int model_;

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
	Vector3 moveVec_;

	float speed_;

	int aliveTime;
	int aliveCounter_;

	int ALIVE_HIT_NUM;
	int aliveHitCou_;
	//Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

	const Vector3 CARRY_OBJ_LOCAL_POS = { 100.0f,80.0f,0.0f };

	const Vector3 LOCAL_THROW_POS = { 0.0f,50.0f,100.0f };
	const Vector3 THROW_VEC = { 0.0f,0.0f,1.0f };

	const Vector3& playerPos_;
	const Vector3& playerAngle_;
};