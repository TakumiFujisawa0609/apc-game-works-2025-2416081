#pragma once

#include"ColliderBase.h"

class ColliderAabb : public ColliderBase
{
public:
	ColliderAabb(TAG type, Vector3 size, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }) :
		ColliderBase(type, enoughDistance, pos),
		size_(size)
	{
	}
	~ColliderAabb()override {}

	Vector3 GetSize(void)const { return size_; }

private:
	Vector3 size_;
};