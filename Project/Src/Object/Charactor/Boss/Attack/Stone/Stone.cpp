#include"Stone.h"

#include"../../../../Common/Collider/SphereCollider.h"

Stone::Stone(int model):
	moveVec(),
	aliveCounter()
{
	ModelDuplicate(model);
}

Stone::~Stone()
{
}

void Stone::Load(void)
{
	ColliderCreate(new SphereCollider(TAG::GOLEM_ATTACK_STONE, RADIUS, RADIUS));

	trans.scale = 1.3f;

	trans.Attach();
}

void Stone::SubInit(void)
{
	SetDynamicFlg(true);
	SetGravityFlg(false);
	SetPushFlg(false);

	SetJudge(false);
	SetIsDraw(false);

	aliveCounter = 0;
}

void Stone::SubUpdate(void)
{
	if (GetJudgeFlg() == false) { return; }

	if (--aliveCounter <= 0) {
		aliveCounter = 0;
		SetJudge(false);
		SetIsDraw(false);
	}

	trans.pos += moveVec;
}

void Stone::On(const Vector3& pos, const Vector3& moveVec)
{
	trans.pos = pos;
	this->moveVec = moveVec * MOVE_SPEED;

	aliveCounter = ALIVE_TIME;

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