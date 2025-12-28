#include"Stone.h"

#include"../../../../Common/Collider/SphereCollider.h"

Stone::Stone(int model):
	moveVec_(),
	aliveCounter_()
{
	ModelDuplicate(model);
}

Stone::~Stone()
{
}

void Stone::Load(void)
{
	ColliderCreate(new SphereCollider(TAG::GOLEM_ATTACK_STONE, RADIUS, RADIUS));

	trans_.scale = 1.3f;

	trans_.Attach();
}

void Stone::SubInit(void)
{
	SetDynamicFlg(true);
	SetGravityFlg(false);
	SetPushFlg(false);

	SetJudge(false);
	SetIsDraw(false);

	aliveCounter_ = 0;
}

void Stone::SubUpdate(void)
{
	if (GetJudgeFlg() == false) { return; }

	if (--aliveCounter_ <= 0) {
		aliveCounter_ = 0;
		SetJudge(false);
		SetIsDraw(false);
	}

	trans_.pos += moveVec_;
}

void Stone::On(const Vector3& pos, const Vector3& moveVec)
{
	trans_.pos = pos;
	moveVec_ = moveVec * MOVE_SPEED;

	aliveCounter_ = ALIVE_TIME;

	SetJudge(true);
	SetIsDraw(true);
}

void Stone::OnCollision(const ColliderBase& collider)
{
	if (
		collider.GetTag() == TAG::PLAYER ||
		collider.GetTag() == TAG::PLAYER_PUNCH
		) {
		SetJudge(false);
		SetIsDraw(false);
	}
}