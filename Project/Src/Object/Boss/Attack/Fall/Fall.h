#pragma once
#include"../../../UnitBase.h"

class Fall : public UnitBase
{
public:
	Fall();
	~Fall();

	void ModelLoad(int model) { unit_.model_ = MV1DuplicateModel(model); }
	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override;

	void On(const VECTOR& pos);

	enum class STATE
	{
		IDLE,
		FALL,

		MAX
	};

private:
	STATE state_;
	
	const int IDLE_TIME = 150;
	int idleCounter_;

	float groundHeight_;
};
