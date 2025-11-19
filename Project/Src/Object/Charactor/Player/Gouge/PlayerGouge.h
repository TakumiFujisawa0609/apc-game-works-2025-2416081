#pragma once

#include"../../../ActorBase.h"

class PlayerGouge : public ActorBase
{
public:
	enum class STATE
	{
		NON = -1,

		SEARCH,
		GOUGE,

		MAX
	};

	PlayerGouge(const Vector3& playerPos, const Vector3& playerAngle);
	~PlayerGouge()override {}

	void Load(void)override;
	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override {}
	void SubRelease(void)override {}

	void OnCollision(const ColliderBase& collider)override;

	int GetState(void)const { return (int)state_; }

	void On(void);
	void Off(void);
	bool Gouge(void);

private:
	STATE state_;

	const float STATE_RADIUS[(int)STATE::MAX] = { 10.0f,150.0f };

	float xAngle_;

	bool searchHit_;
	bool gougeHit_;

	const Vector3 LOCAL_POS = { 0.0f,150.0f,0.0f };
	const Vector3 FOOT_POS = { 0.0f,-90.0f,0.0f };
	const Vector3& playerPos;
	const Vector3& playerAngle;
};