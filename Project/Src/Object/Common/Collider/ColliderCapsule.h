#pragma once

#include"ColliderBase.h"

class ColliderCapsule : public ColliderBase
{
public:
	ColliderCapsule(TAG type,float halfLen,float radius, Vector3 pos = { 0.0f, 0.0f, 0.0f }, float enoughDistance = -1.0f):
		ColliderBase(type,pos,enoughDistance),
		halfLen_(halfLen),
		radius_(radius)
	{
	}
	~ColliderCapsule()override {}

	float GetHalfLen(void)const { return halfLen_; }
	float GetRadius(void)const { return radius_; }
private:
	float halfLen_;
	float radius_;
};