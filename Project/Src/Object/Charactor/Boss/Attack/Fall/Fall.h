#pragma once
#include"../../../../ActorBase.h"

class Fall : public ActorBase
{
public:
	Fall(int model);
	~Fall();

	void Load(void)override;

	void OnCollision(const ColliderBase& collider)override;

	enum class STATE
	{
		IDLE,
		FALL,

		MAX
	};

	void Set(const Vector3& pos);
	void On(void) { state_ = STATE::FALL; }

	STATE GetState(void)const { return state_; }

private:

	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override {}
	void SubAlphaDraw(void)override;
	void SubRelease(void)override {}

	STATE state_;
	
	const int IDLE_TIME = 1000;
	int idleCounter_;

	float groundHeight_;
};
