#include"Stone.h"


Stone::Stone()
{
}

Stone::~Stone()
{
}

void Stone::Load(void)
{
}

void Stone::Init(void)
{
	unit_.isAlive_ = false;
}

void Stone::Update(void)
{
	if (unit_.isAlive_ == false) { return; }

	unit_.pos_ = VAdd(unit_.pos_, moveVec_);

}

void Stone::Draw(void)
{
	if (unit_.isAlive_ == false) { return; }

	Utility::MV1ModelMatrix(unit_.model_, unit_.pos_, { unit_.angle_ });
	MV1DrawModel(unit_.model_);
}

void Stone::Release(void)
{
	MV1DeleteModel(unit_.model_);
}

void Stone::On(const VECTOR& pos, const VECTOR& moveVec)
{
	unit_.pos_ = pos;
	moveVec_ = VScale(moveVec, unit_.para_.speed);

	unit_.isAlive_ = true;
}

void Stone::OnCollision(UnitBase* other)
{
}

