#include"ColliderBase.h"

#include"ColliderLine.h"
#include"ColliderSphere.h"
#include"ColliderCapsule.h"
#include"ColliderAabb.h"

ColliderBase::ColliderBase(TAG type, float enoughDistance, Vector3 pos) :
	trans_(nullptr),
	enoughDistance_(enoughDistance),
	pos_(pos),
	type_(type),
	shape_(SHAPE::NON),
	OnCollision(nullptr)
{
	// é©ï™ÇÃÉNÉâÉXÇîªï ÇµÇƒå`èÛÇï€éùÇ∑ÇÈ
	if (dynamic_cast<ColliderLine*>(this)) { shape_ = SHAPE::LINE; }
	else if (dynamic_cast<ColliderSphere*>(this)) { shape_ = SHAPE::SPHERE; }
	else if (dynamic_cast<ColliderCapsule*>(this)) { shape_ = SHAPE::CAPSULE; }
	else if (dynamic_cast<ColliderAabb*>(this)) { shape_ = SHAPE::AABB; }
	//else if (dynamic_cast<ColliderModel*>(this)) { shape_ = SHAPE::MODEL; }
	//else if (dynamic_cast<ColliderVoxel*>(this)) { shape_ = SHAPE::VOXEL; }
}
void ColliderBase::DrawDebug(unsigned int color)
{
	if (ColliderLine* coll = dynamic_cast<ColliderLine*>(this)) {

	}
	else if (ColliderSphere* coll = dynamic_cast<ColliderSphere*>(this)) {
	}
	else if (ColliderCapsule* coll = dynamic_cast<ColliderCapsule*>(this)) {
	}
	else if (ColliderAabb* coll = dynamic_cast<ColliderAabb*>(this)) {
	}
	//else if (ColliderModel* coll = dynamic_cast<ColliderModel*>(this)) {
	// }
	//else if (ColliderVoxel* coll = dynamic_cast<ColliderVoxel*>(this)) {
	// }
}