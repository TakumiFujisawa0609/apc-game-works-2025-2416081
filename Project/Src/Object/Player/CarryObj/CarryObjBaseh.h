#pragma once
#include"../../UnitBase.h"

class CarryObjBase
{
public:
	CarryObjBase(int model,const VECTOR& playerPos_, const VECTOR& playerAngle_);
	~CarryObjBase();

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	void On(void){ state_ = STATE::CARRY; }
	void Off(void) { state_ = STATE::NON; }
	void DropObj(void);

	enum class STATE
	{
		NON,
		CARRY,
		DROP,

		MAX
	};

	STATE GetState(void)const { return state_; }

private:
	int model_;

	STATE state_;

	VECTOR pos_;
	VECTOR angle_;

	// ä÷êîÉ|ÉCÉìÉ^îzóÒ
	using STATEFUNC = void (CarryObjBase::*)(void);
	STATEFUNC stateFuncPtr[(int)STATE::MAX];

	// èÛë‘ï ä÷êîÅ`Å`Å`Å`
	void Non(void) {};
	void Carry(void);
	void Drop(void);
	//Å`Å`Å`Å`Å`Å`Å`Å`Å`

	const VECTOR CARRY_OBJ_LOCAL_POS = { 0.0f,20.0f,70.0f };
	const VECTOR& playerPos_;
	const VECTOR& playerAngle_;
};
