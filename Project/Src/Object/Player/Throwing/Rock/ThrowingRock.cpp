#include"ThrowingRock.h"

ThrowingRock::ThrowingRock():
	aliveCounter_(0),
	moveVec_()
{
}

ThrowingRock::~ThrowingRock()
{
}

void ThrowingRock::Load(void)
{
	unit_.model_ = MV1LoadModel("Data/Model/Player/ThrowingObj/Rock/Rock.mv1");
}

void ThrowingRock::Init(void)
{
}

void ThrowingRock::Update(void)
{
	if (!unit_.isAlive_) { return; }

	unit_.pos_ = VAdd(unit_.pos_, moveVec_);

	if (--aliveCounter_ <= 0) {
		aliveCounter_ = 0;
		unit_.isAlive_ = false;
	}
}

void ThrowingRock::Draw(void)
{
	if (!unit_.isAlive_) { return; }

	Utility::MV1ModelMatrix(unit_.model_, unit_.pos_, { unit_.angle_ });
	MV1DrawModel(unit_.model_);
}

void ThrowingRock::Release(void)
{
	MV1DeleteModel(unit_.model_);
}

void ThrowingRock::OnCollision(UnitBase* other)
{
}

void ThrowingRock::Throw(const VECTOR& pos, const VECTOR& vec)
{
	unit_.pos_ = pos;
	moveVec_ = VScale(vec,unit_.para_.speed);

	aliveCounter_ = ALIVE_TIME;

	unit_.isAlive_ = true;
}
