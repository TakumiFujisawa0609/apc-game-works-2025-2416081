#pragma once

#include"ColliderBase.h"

class ColliderSphere : public ColliderBase
{
public:
	ColliderSphere(TAG type, float radius, Vector3 pos = { 0.0f, 0.0f, 0.0f }, float enoughDistance = -1.0f) :
		ColliderBase(type,pos,enoughDistance),
		radius_(radius)
	{
	}
	~ColliderSphere()override {}

	float GetRadius(void)const { return radius_; }

private:
	float radius_;
};