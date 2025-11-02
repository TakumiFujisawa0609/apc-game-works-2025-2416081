#include"Fall.h"

#include"../../../../Manager/Sound/SoundManager.h"

#include"../../../Player/Player.h"

Fall::Fall()
{
}

Fall::~Fall()
{
}

void Fall::Load(void)
{
	unit_.para_.colliShape = CollisionShape::SPHERE;
	unit_.para_.radius = 150.0f;

	unit_.para_.speed = 30.0f;
}

void Fall::Init(void)
{
}

void Fall::Update(void)
{
	if (!unit_.isAlive_) { return; }

	switch (state_)
	{
	case Fall::STATE::IDLE:
		if (++idleCounter_ > IDLE_TIME) {
			state_ = STATE::FALL;
		}
		break;
	case Fall::STATE::FALL:
		unit_.pos_.y -= unit_.para_.speed;
		if (unit_.pos_.y < -100.0f) { unit_.isAlive_ = false; }
		break;
	}

}

void Fall::Draw(void)
{
	if (!unit_.isAlive_) { return; }

	if (state_ == STATE::IDLE && idleCounter_ / 10 % 2 == 0) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
		DrawSphere3D({ unit_.pos_.x,500.0f,unit_.pos_.z }, unit_.para_.radius, 4, 0xff0000, 0xff0000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	Utility::MV1ModelMatrix(unit_.model_, unit_.pos_, { unit_.angle_ });
	MV1DrawModel(unit_.model_);
}

void Fall::Release(void)
{
	MV1DeleteModel(unit_.model_);
}

void Fall::OnCollision(UnitBase* other)
{
	if (
		dynamic_cast<PlayerPunch*>(other) ||
		dynamic_cast<ThrowObjBase*>(other)
		) {
		unit_.isAlive_ = false;
	}
}

void Fall::On(const VECTOR& pos)
{
	unit_.pos_ = pos;
	idleCounter_ = 0;
	state_ = STATE::IDLE;
	unit_.isAlive_ = true;
}
