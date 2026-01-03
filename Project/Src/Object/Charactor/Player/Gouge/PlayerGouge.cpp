#include"PlayerGouge.h"

#include"../../../../Manager/Sound/SoundManager.h"

PlayerGouge::PlayerGouge(const Vector3& playerPos, const Vector3& playerAngle) :
	ActorBase(),

	state_(STATE::NON),

	xAngle_(0.0f),

	searchHit_(false),
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
	ColliderCreate(new SphereCollider(TAG::PLAYER_GOUGE, STATE_RADIUS[(int)state_], STATE_RADIUS[(int)STATE::GOUGE]));
}

void PlayerGouge::SubInit(void)
{
	searchHit_ = false;
	gougeHit_ = false;
	Off();
}

void PlayerGouge::SubUpdate(void)
{
	if (!GetJudgeFlg()) { return; }

	ColliderSerch<SphereCollider>().back()->SetRadius(STATE_RADIUS[(int)state_]);

	switch (state_)
	{
	case PlayerGouge::STATE::SEARCH:
		if (searchHit_) { return; }
		xAngle_ += Utility::Deg2RadF(5.0f);
		if (xAngle_ > Utility::Deg2RadF(200.0f)) { xAngle_ = Utility::Deg2RadF(200.0f); }
		else {
			trans_.pos = (playerPos + FOOT_POS) + LOCAL_POS.TransMat(Utility::MatrixAllMultXYZ({ Vector3::Xonly(xAngle_) , playerAngle }));
		}
		break;
	case PlayerGouge::STATE::GOUGE:
		if(gougeHit_){
			SetJudge(false);
			//SetIsDraw(false);
			state_ = STATE::NON;
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

	switch (state_)
	{
	case PlayerGouge::STATE::SEARCH:
		searchHit_ = true;
		return;
	case PlayerGouge::STATE::GOUGE:

		if (!gougeHit_) {
			gougeHit_ = true;
			Smng::GetIns().Play(SOUND::OBJECT_BREAK, true, 150);
		}
		return;
	}
}

void PlayerGouge::On(void)
{
	if (GetJudgeFlg()) { return; }

	SetJudge(true);
	SetIsDraw(true);
	searchHit_ = false;
	gougeHit_ = false;
	state_ = STATE::SEARCH;
	xAngle_ = 0.0f;

	trans_.pos = (playerPos + FOOT_POS) + LOCAL_POS.TransMat(Utility::MatrixAllMultXYZ({ Vector3::Xonly(xAngle_) , playerAngle }));
}

void PlayerGouge::Off(void)
{
	SetJudge(false);
	//SetIsDraw(false);
	state_ = STATE::NON;
}

bool PlayerGouge::Gouge(void)
{
	if (searchHit_) {
		state_ = STATE::GOUGE;
		gougeHit_ = false;
		return true;
	}

	Off();
	return false;
}
