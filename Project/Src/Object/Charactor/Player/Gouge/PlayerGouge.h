#pragma once

#include"../../../ActorBase.h"

class PlayerGouge : public ActorBase
{
public:
	enum class STATE
	{
		NON = -1,

		SEARCH,
		GOUGE,

		MAX
	};

	PlayerGouge(const int& playerModel);
	~PlayerGouge()override = default;

	void Load(void)override;

	void OnCollision(const ColliderBase& collider)override;

	int GetState(void)const { return (int)state; }

	void SearchOn(void) { state = STATE::SEARCH; searchHit = false; On(); SubUpdate(); }
	void GougeOn(void) { state = STATE::GOUGE; }

	bool SearchHit(void) { return searchHit; }

	void Reset(void);

private:
	const int PLAYER_RIGHTHAND_FRAME_INDEX = 14;

	void SubInit(void)override;
	void SubUpdate(void)override;

	void On(void) { SetJudge(true); SetIsDraw(true); }

	STATE state;

	// èÛë‘ï îºåa
	const float STATE_RADIUS[(int)STATE::MAX] = { 30.0f,120.0f };

	bool searchHit;
	bool gougeHit;

	const int& playerModel;
};