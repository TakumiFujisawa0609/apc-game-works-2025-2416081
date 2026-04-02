#include"PlayerGouge.h"

#include"../../../../Manager/Sound/SoundManager.h"

#include"../../../Common/Collider/SphereCollider.h"

PlayerGouge::PlayerGouge(const int& playerModel, int PLAYER_RIGHTHAND_FRAME_INDEX):
	ActorBase(),

	playerModel(playerModel),
	PLAYER_RIGHTHAND_FRAME_INDEX(PLAYER_RIGHTHAND_FRAME_INDEX),

	state(STATE::NON),

	searchHit(false),
	gougeHit(false)
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
	gougeHit = false;
	Reset();
}

void PlayerGouge::SubUpdate(void)
{
	if (state == STATE::NON) { return; }

	ColliderSerch<SphereCollider>().back()->SetRadius(STATE_RADIUS[(int)state]);

	switch (state)
	{
	case PlayerGouge::STATE::SEARCH:
		if (searchHit) { return; }
		trans.pos = MV1GetFramePosition(playerModel, PLAYER_RIGHTHAND_FRAME_INDEX);
		break;
	case PlayerGouge::STATE::GOUGE:
		if (gougeHit) { Reset(); }
		break;
	}
}

void PlayerGouge::OnCollision(const ColliderBase& collider)
{
	const TAG& otherTag = collider.GetTag();

	if (
		otherTag != TAG::STAGE &&
		otherTag != TAG::GOLEM_ATTACK_WALL
		)
	{
		return;
	}

	switch (state)
	{
	case PlayerGouge::STATE::SEARCH: {
		searchHit = true;
		gougeHit = false;
		return;
	}
	case PlayerGouge::STATE::GOUGE: {
		gougeHit = true;
		Snd::GetIns().Play("ObjBreak");
		return;
	}
	}
}

void PlayerGouge::Reset(void)
{
	SetJudge(false);
	SetIsDraw(false);
	state = STATE::NON;

	searchHit = false;
	gougeHit = false;
}