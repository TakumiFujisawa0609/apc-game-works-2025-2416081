#pragma once

#include"../../../ActorBase.h"

class ThrowObjBase : public ActorBase
{
public:
	ThrowObjBase(const int& playerModel, const Vector3& playerAngle, int PLAYER_RIGHTHAND_FRAME_INDEX, const std::string& parameterPath);
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

	const int& playerModel;
	const Vector3& playerAngle;

	Vector3 PlayerRightHandPos(void)const { return Vector3(MV1GetFramePosition(playerModel, PLAYER_RIGHTHAND_FRAME_INDEX)); }

	void SubInit(void)override;
	void SubUpdate(void)override;

	// “Š‚°‚½Œã‚Ìˆ—‚Ég‚¤``````
	Vector3 moveVec;

	// ‰Eè‚ÌƒtƒŒ[ƒ€ƒCƒ“ƒfƒbƒNƒX(ƒ‚ƒfƒ‹‚É‚æ‚Á‚Ä•Ï‚¦‚é•K—v‚ª‚ ‚é‚©‚à)(”h¶æ‚Åİ’è)
	const int PLAYER_RIGHTHAND_FRAME_INDEX;

	// ‘¬“x(”h¶æ‚Åİ’è)
	const float SPEED = GetParameter("Speed");

	// ¶‘¶ŠÔ(”h¶æ‚Åİ’è)
	const int ALIVE_TIME = GetParameterToInt("AliveTime");
	int aliveCounter;

	// ¶‘¶ƒqƒbƒg‰ñ”(”h¶æ‚Åİ’è)
	const int ALIVE_HIT_NUM = GetParameterToInt("AliveHitNum");
	int aliveHitCou;

	// —‰º‘¬“x(”h¶æ‚Åİ’è)
	const float DROP_SPEED = GetParameter("DropSpeed");

	// —‰º’†ƒIƒuƒWƒFƒNƒg‚ğÁ‹‚·‚é‚‚³(”h¶æ‚Åİ’è)
	const float DROP_DELETE_HEIGHT = GetParameter("DropDeleteHeight");
	//````````````````

protected:
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

	const Vector3 THROW_VEC = Vector3::Zonly(1.0f);
};