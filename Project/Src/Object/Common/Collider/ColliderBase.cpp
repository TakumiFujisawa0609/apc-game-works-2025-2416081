#include"ColliderBase.h"

#include"LineCollider.h"
#include"SphereCollider.h"
#include"CapsuleCollider.h"
#include"BoxCollider.h"
#include"VoxelCollider.h"


ColliderBase::ColliderBase(TAG type, float enoughDistance, Vector3 pos) :
	trans_(nullptr),
	pos_(pos),
	enoughDistance_(enoughDistance),
	judgeFlg_(1),
	dynamicFlg_(1),
	pushFlg_(1),
	pushWeight_(0),
	type_(type),
	shape_(SHAPE::NON),
	OnCollision(nullptr)
{
}
void ColliderBase::DrawDebug(unsigned int color)
{
	if (LineCollider* coll = dynamic_cast<LineCollider*>(this)) {

	}
	else if (SphereCollider* coll = dynamic_cast<SphereCollider*>(this)) {
	}
	else if (CapsuleCollider* coll = dynamic_cast<CapsuleCollider*>(this)) {
	}
	else if (BoxCollider* coll = dynamic_cast<BoxCollider*>(this)) {
	}
	//else if (ColliderModel* coll = dynamic_cast<ColliderModel*>(this)) {
	// }
	else if (VoxelCollider* coll = dynamic_cast<VoxelCollider*>(this)) {
	 }
}