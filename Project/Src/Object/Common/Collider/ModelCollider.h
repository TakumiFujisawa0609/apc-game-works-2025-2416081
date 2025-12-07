#pragma once

#include"ColliderBase.h"

class ModelCollider : public ColliderBase
{
public:
	ModelCollider(TAG type, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }) :
		ColliderBase(type, enoughDistance, pos)
	{
		SetShape(SHAPE::MODEL);
	}
	~ModelCollider()override {}


private:

};