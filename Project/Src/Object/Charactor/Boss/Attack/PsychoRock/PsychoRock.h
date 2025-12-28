#pragma once

#include"../../../../ActorBase.h"

class PsychoRock : public ActorBase
{
public:
	PsychoRock(int model, const Vector3& playerPos);
	~PsychoRock();

	void Load(void)override;

	enum class STATE {
		NON,
		RISE_PREPARA,
		RISE,
		SHOT,

		MAX,
	};
	STATE GetState(void)const { return state_; }

	void Set(float x, float z);
	void On(void);

private:

	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override {}
	void SubAlphaDraw(void)override;
	void SubRelease(void)override {}

	STATE state_;

	// 関数ポインタ配列
	using STATEFUNC = void (PsychoRock::*)(void);

	void Non(void) {};

	STATEFUNC stateUpdateFuncPtr[(int)STATE::MAX];
	void RisePreparaUpdate(void);
	void RiseUpdate(void);
	void ShotUpdate(void);

	STATEFUNC stateDrawFuncPtr[(int)STATE::MAX];
	void RisePreparaDraw(void);
	void RiseDraw(void);
	void ShotDraw(void);

	const float SCALE = 1.5f;
	const float RADIUS = 65.0f * SCALE;

	const float MOVE_SPEED = 30.0f;

	const int PREPARA_TIME = 500;
	int preparaTimer_;

	const float RISE_SPEED = 10.0f;

	Vector3 moveVec_;
	Vector3 targetPos_;

	const Vector3& playerPos;
};