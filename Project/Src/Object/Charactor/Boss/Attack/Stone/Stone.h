#pragma once

#include"../../../../ActorBase.h"

class Stone : public ActorBase
{
public:
	Stone(int model);
	~Stone();

	void Load(void)override;

	void On(const Vector3& pos, const Vector3& moveVec);

	virtual void OnCollision(const ColliderBase& collider);

private:
	const float RADIUS = 100.0f;

	const float MOVE_SPEED = 30.0f;

	Vector3 moveVec_;

	const int ALIVE_TIME = 200;
	int aliveCounter_;

	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override {}
	void SubAlphaDraw(void)override {}
	void SubRelease(void)override {}
};
