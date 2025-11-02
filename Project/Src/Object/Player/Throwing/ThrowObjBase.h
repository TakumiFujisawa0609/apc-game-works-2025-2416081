#pragma once

#include"../../UnitBase.h"

class ThrowObjBase : public UnitBase
{
public:
	ThrowObjBase(const VECTOR& playerPos_, const VECTOR& playerAngle_);
	virtual ~ThrowObjBase() = 0;

	void ModelLoad(int model);
	virtual void Load(void)override;
	virtual void Init(void)override;
	virtual void Update(void)override;
	virtual void Draw(void)override;
	virtual void Release(void)override;

	virtual void OnCollision(UnitBase* other)override;

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

	int GetState(void)const override { return (int)state_; }

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
	VECTOR moveVec_;

	int aliveTime;
	int aliveCounter_;

	int ALIVE_HIT_NUM;
	int aliveHitCou_;
	//Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`



	const VECTOR CARRY_OBJ_LOCAL_POS = { 0.0f,20.0f,70.0f };

	const VECTOR LOCAL_THROW_POS = { 0.0f,50.0f,100.0f };
	const VECTOR THROW_VEC = { 0.0f,0.0f,1.0f };

	const VECTOR& playerPos_;
	const VECTOR& playerAngle_;
};