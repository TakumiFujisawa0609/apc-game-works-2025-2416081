#pragma once

#include"ColliderBase.h"

class SphereCollider : public ColliderBase
{
public:
	SphereCollider(TAG type, float radius, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }) :
		ColliderBase(type, enoughDistance, pos),
		radius_(radius)
	{
	}
	~SphereCollider()override {}

	float GetRadius(void)const { return radius_; }
	void SetRadius(float radius) { radius_ = radius; }

private:
	float radius_;
};