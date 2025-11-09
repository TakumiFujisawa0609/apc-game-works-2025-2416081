#pragma once

#include"../../../UnitBase.h"

class PsychoRock : public UnitBase
{
public:
	PsychoRock(int model, const VECTOR& playerPos);
	~PsychoRock();

	void Load(void)override;
	void Init(void)override {};
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override;

	enum class STATE {
		NON,
		RISE_PREPARA,
		RISE,
		SHOT,

		MAX,
	};
	int GetState(void)const override { return (int)state_; }

	void Set(float x, float z);
	void On(void);

private:
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


	const int PREPARA_TIME = 500;
	int preparaTimer_;

	const float RISE_SPEED = 10.0f;

	VECTOR moveVec_;
	VECTOR targetPos_;

	const VECTOR& playerPos;
};