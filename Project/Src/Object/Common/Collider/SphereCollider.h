#pragma once

#include"ColliderBase.h"

class SphereCollider : public ColliderBase
{
public:
	SphereCollider(TAG type, float radius, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }) :
		ColliderBase(type, enoughDistance, pos),
		radius_(radius)
	{
		SetShape(SHAPE::SPHERE);
	}
	~SphereCollider()override {}

	float GetRadius(void)const { return radius_; }
	void SetRadius(float radius) { radius_ = radius; }

	void DrawDebug(unsigned int color = 0xffffff)override {
		DrawSphere3D(GetPos().ToVECTOR(), radius_, 12, color, color, true);
	}

private:
	float radius_;
};