#pragma once

#include"ColliderBase.h"

class SphereCollider : public ColliderBase
{
public:
	SphereCollider(TAG type, float radius, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }) :
		ColliderBase(type, enoughDistance, pos),
		radius(radius)
	{
		SetShape(SHAPE::SPHERE);
	}
	~SphereCollider()override {}

	float GetRadius(void)const { return radius; }
	void SetRadius(float radius) { this->radius = radius; }

	void DrawDebug(unsigned int color = 0xffffff)override {
		DrawSphere3D(GetPos().ToVECTOR(), radius, 12, color, color, true);
	}

private:
	float radius;
};