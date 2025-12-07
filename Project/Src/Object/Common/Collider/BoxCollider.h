#pragma once

#include"ColliderBase.h"

class BoxCollider : public ColliderBase
{
public:
	BoxCollider(TAG type, Vector3 size, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }) :
		ColliderBase(type, enoughDistance, pos),
		size_(size)
	{
		SetShape(SHAPE::BOX);
	}
	~BoxCollider()override {}

	Vector3 GetSize(void)const { return size_; }

private:
	Vector3 size_;
};