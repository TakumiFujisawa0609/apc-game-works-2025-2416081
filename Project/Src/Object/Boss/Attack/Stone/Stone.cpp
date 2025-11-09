#include"Stone.h"

#include"../../../Player/Player.h"


Stone::Stone():
	moveVec_(),
	aliveCounter_()
{
}

Stone::~Stone()
{
}

void Stone::Load(void)
{
	unit_.para_.colliShape = CollisionShape::SPHERE;
	unit_.para_.radius = 100.0f;
	MV1SetScale(unit_.model_, Utility::FtoV(1.3f));

	unit_.para_.speed = 30.0f;
}

void Stone::Init(void)
{
	unit_.isAlive_ = false;
	aliveCounter_ = 0;
}

void Stone::Update(void)
{
	if (unit_.isAlive_ == false) { return; }

	if (--aliveCounter_ <= 0) {
		aliveCounter_ = 0;
		unit_.isAlive_ = false;
	}

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

	aliveCounter_ = ALIVE_TIME;

	unit_.isAlive_ = true;
}

void Stone::OnCollision(UnitBase* other)
{
	if (
		dynamic_cast<Player*>(other) ||
		dynamic_cast<PlayerPunch*>(other)
		) {
		unit_.isAlive_ = false;
	}
}

