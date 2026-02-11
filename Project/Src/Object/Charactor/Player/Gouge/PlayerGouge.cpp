#include"PlayerGouge.h"

#include"../../../../Manager/Sound/SoundManager.h"

#include"../../../Common/Collider/SphereCollider.h"

PlayerGouge::PlayerGouge(const Vector3& playerPos, const Vector3& playerAngle) :
	ActorBase(),

	state(STATE::NON),

	xAngle(0.0f),

	searchHit(false),
	gougeHit_(false),

	playerPos(playerPos),
	playerAngle(playerAngle)
{
}

void PlayerGouge::Load(void)
{
	SetDynamicFlg(true);
	SetGravityFlg(false);

	SetPushFlg(false);

	// コライダー生成
	ColliderCreate(new SphereCollider(TAG::PLAYER_GOUGE, STATE_RADIUS[(int)state], STATE_RADIUS[(int)STATE::GOUGE]));
}

void PlayerGouge::SubInit(void)
{
	searchHit = false;
	gougeHit_ = false;
	Off();
}

void PlayerGouge::SubUpdate(void)
{
	if (!GetJudgeFlg()) { return; }

	ColliderSerch<SphereCollider>().back()->SetRadius(STATE_RADIUS[(int)state]);

	switch (state)
	{
	case PlayerGouge::STATE::SEARCH:
		if (searchHit) { return; }
		xAngle += Deg2Rad(5.0f);
		if (xAngle > Deg2Rad(120.0f)) { xAngle = Deg2Rad(120.0f); }
		else {
			trans.pos = (playerPos + FOOT_POS) + LOCAL_POS.TransMat(MatrixAllMultXYZ({ Vector3::Xonly(xAngle) , playerAngle }));
		}
		break;
	case PlayerGouge::STATE::GOUGE:
		if(gougeHit_){
			SetJudge(false);
			SetIsDraw(false);
			state = STATE::NON;
		}
		break;
	}
}

void PlayerGouge::OnCollision(const ColliderBase& collider)
{
	TAG otherTag = collider.GetTag();

	if (
		otherTag != TAG::STAGE &&
		otherTag != TAG::GOLEM_ATTACK_WALL
		)
	{
		return;
	}

	switch (state)
	{
	case PlayerGouge::STATE::SEARCH:
		searchHit = true;
		return;
	case PlayerGouge::STATE::GOUGE:

		if (!gougeHit_) {
			gougeHit_ = true;
			Snd::GetIns().Play("ObjBreak");
		}
		return;
	}
}

void PlayerGouge::On(void)
{
	if (GetJudgeFlg()) { return; }

	SetJudge(true);
	SetIsDraw(true);
	searchHit = false;
	gougeHit_ = false;
	state = STATE::SEARCH;
	xAngle = 0.0f;

	trans.pos = (playerPos + FOOT_POS) + LOCAL_POS.TransMat(MatrixAllMultXYZ({ Vector3::Xonly(xAngle) , playerAngle }));
}

void PlayerGouge::Off(void)
{
	SetJudge(false);
	SetIsDraw(false);
	state = STATE::NON;
}

bool PlayerGouge::Gouge(void)
{
	if (searchHit) {
		state = STATE::GOUGE;
		gougeHit_ = false;
		return true;
	}

	Off();
	return false;
}
