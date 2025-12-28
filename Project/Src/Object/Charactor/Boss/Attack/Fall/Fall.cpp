#include"Fall.h"

#include"../../../../../Manager/Sound/SoundManager.h"
#include"../../../../../Manager/Collision/CollisionManager.h"

#include"../../../../Common/Collider/SphereCollider.h"

Fall::Fall(int model):
	state_(),

	idleCounter_(),
	groundHeight_()
{
	ModelDuplicate(model);
}

Fall::~Fall()
{
}

void Fall::Load(void)
{
	trans_.scale = SCALE;

	ColliderCreate(new SphereCollider(TAG::GOLEM_ATTACK_FALL, RADIUS, RADIUS));
}

void Fall::SubInit(void)
{
	SetDynamicFlg(true);
	SetGravityFlg(false);
	SetPushFlg(false);

	SetJudge(false);
	SetIsDraw(false);
}

void Fall::SubUpdate(void)
{
	if (!GetJudgeFlg()) { return; }

	switch (state_)
	{
	case STATE::IDLE:
		if (++idleCounter_ > IDLE_TIME) {
			state_ = STATE::FALL;
		}
		break;
	case STATE::FALL:
		trans_.pos.y -= 30.0f;
		if (trans_.pos.y < -100.0f) { SetJudge(false); SetIsDraw(false); }
		break;
	}
}

void Fall::SubAlphaDraw(void)
{
	if (state_ == STATE::IDLE && idleCounter_ / 10 % 2 == 0) {
		VECTOR predictionPos = { trans_.pos.x,groundHeight_,trans_.pos.z };
		float radius = ColliderSerch<SphereCollider>().back()->GetRadius();
		DrawSphere3D(predictionPos, radius * 1.3f, 4, 0xff0000, 0xff0000, true);
	}
}

void Fall::OnCollision(const ColliderBase& collider)
{
	switch (collider.GetTag())
	{
	case TAG::PLAYER_PUNCH:
	case TAG::PLAYER_THROWING: {
		SetJudge(false);
		SetIsDraw(false);
		return;
	}

	default: { return; }
	}
}

void Fall::Set(const Vector3& pos)
{
	trans_.pos = pos;
	idleCounter_ = 0;
	state_ = STATE::IDLE;

	SetJudge(true);
	SetIsDraw(true);

	groundHeight_ = 0.0f;

	Vector3 work = trans_.pos;
	work.y = 500.0f;

	float radius = ColliderSerch<SphereCollider>().back()->GetRadius();

	while (work.y > 0.0f) {
		if (CollisionManager::IsStageCollision(work, radius)) { break; }
		work.y -= 10.0f;
		groundHeight_ = work.y - radius;
	}
}