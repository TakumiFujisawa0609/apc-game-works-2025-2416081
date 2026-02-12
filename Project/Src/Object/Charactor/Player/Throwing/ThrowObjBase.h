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

	void Carry(void) { state = STATE::CARRY; }
	void Drop(void) { state = STATE::DROP; }
	virtual void Throw(void);

	STATE GetState(void)const { return state; }

private:
	const int PLAYER_RIGHTHAND_FRAME_INDEX = 14;
	const int& playerModel;
	const Vector3& playerAngle;

	const Vector3& PlayerRightHandPos(void)const { return Vector3(MV1GetFramePosition(playerModel, PLAYER_RIGHTHAND_FRAME_INDEX)); }

	void SubInit(void)override;
	void SubUpdate(void)override;

protected:
	int model;

	// ó‘Ô
	STATE state;

	// ŠÖ”ƒ|ƒCƒ“ƒ^’è‹`—p
	using STATEFUNC = void (ThrowObjBase::*)(void);
	// ó‘Ô•ÊŠÖ”ƒ|ƒCƒ“ƒ^”z—ñ
	STATEFUNC stateFuncPtr[(int)STATE::MAX];

	// ó‘Ô•ÊŠÖ”````
	void NonStateFunc(void) {};
	void CarryStateFunc(void);
	void DropStateFunc(void);
	void ThrowStateFunc(void);
	//`````````


	// “Š‚°‚½Œã‚Ìˆ—‚Ég‚¤``````
	Vector3 moveVec;

	float speed;

	// ¶‘¶ŠÔ(”h¶æ‚Åİ’è)
	int aliveTime;
	int aliveCounter;

	// ¶‘¶ƒqƒbƒg‰ñ”(”h¶æ‚Åİ’è)
	int ALIVE_HIT_NUM;
	int aliveHitCou;
	//````````````````

	const Vector3 THROW_VEC = { 0.0f,0.0f,1.0f };

};