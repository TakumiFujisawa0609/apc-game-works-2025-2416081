#pragma once

#include"ColliderBase.h"

class CapsuleCollider : public ColliderBase
{
public:
	CapsuleCollider(TAG type,float halfLen,float radius, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }):
		ColliderBase(type, enoughDistance, pos),
		halfLen_(halfLen),
		radius_(radius)
	{
	}
	~CapsuleCollider()override {}

	float GetHalfLen(void)const { return halfLen_; }
	float GetRadius(void)const { return radius_; }
private:
	float halfLen_;
	float radius_;
};