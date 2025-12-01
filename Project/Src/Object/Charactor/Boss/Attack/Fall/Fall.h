#pragma once
#include"../../../UnitBase.h"

class Fall : public UnitBase
{
public:
	Fall(int model);
	~Fall();

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override;

	enum class STATE
	{
		IDLE,
		FALL,

		MAX
	};

	void Set(const VECTOR& pos);
	void On(void) { state_ = STATE::FALL; }

	int GetState(void)const override { return (int)state_; }

private:
	STATE state_;
	
	const int IDLE_TIME = 1000;
	int idleCounter_;

	float groundHeight_;
};
