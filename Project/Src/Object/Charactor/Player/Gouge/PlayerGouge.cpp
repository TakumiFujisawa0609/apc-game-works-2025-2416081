#include"PlayerGouge.h"

#include"../../../Common/Collider/SphereCollider.h"

#include"../../../Stage/Block/Block.h"
#include"../../Boss/Attack/RockWall/RockWall.h"

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
	// コライダー生成
	ColliderCreate(new SphereCollider(TAG::PLAYER_GOUGE, STATE_RADIUS[(int)state_]));
}

void PlayerGouge::SubInit(void)
{
	searchHit_ = false;
	gougeHit_ = false;
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
			trans_.pos = (playerPos + FOOT_POS) + LOCAL_POS.TransMat(Utility::MatrixAllMultXYZ({ Vector3(xAngle_) , playerAngle }));
		}
		break;
	case PlayerGouge::STATE::GOUGE:
		if(gougeHit_){
			SetJudge(false);
			state_ = STATE::NON;
		}
		break;
	}
}

void PlayerGouge::OnCollision(const ColliderBase& collider)
{
	switch (state_)
	{
	case PlayerGouge::STATE::SEARCH:
		if (searchHit_) { return; }
		if (collider.GetTag() == TAG::STAGE) {
			searchHit_ = true;
			return;
		}
		break;
	case PlayerGouge::STATE::GOUGE:
		if (collider.GetTag() == TAG::STAGE) {
			gougeHit_ = true;
			return;
		}
		break;
	}
}

void PlayerGouge::On(void)
{
	if (GetJudgeFlg()) { return; }

	trans_.pos = playerPos + FOOT_POS;

	SetJudge(true);
	searchHit_ = false;
	gougeHit_ = false;
	state_ = STATE::SEARCH;
	xAngle_ = 0.0f;
}

void PlayerGouge::Off(void)
{
	SetJudge(false);
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
