#pragma once

#include"ColliderBase.h"

class ColliderAabb : public ColliderBase
{
public:
	ColliderAabb(TAG type, Vector3 size, Vector3 pos = { 0.0f, 0.0f, 0.0f }, float enoughDistance = -1.0f) :
		ColliderBase(type, pos, enoughDistance),
		size_(size)
	{
	}
	~ColliderAabb()override {}

	Vector3 GetSize(void)const { return size_; }

private:
	Vector3 size_;
};