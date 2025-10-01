#pragma once
#include"../../../UnitBase.h"
class ThrowingRock : public UnitBase
{
public:
	static constexpr int ALIVE_TIME = 300;

	ThrowingRock();
	~ThrowingRock();

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override;

	void Throw(const VECTOR& pos, const VECTOR& vec);

private:
	VECTOR moveVec_;
	int aliveCounter_;
};
