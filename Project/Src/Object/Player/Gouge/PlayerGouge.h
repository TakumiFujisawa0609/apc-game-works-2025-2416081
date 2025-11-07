#pragma once

#include"../../UnitBase.h"

class PlayerGouge :public UnitBase
{
public:
	enum class STATE
	{
		NON=-1,

		SEARCH,
		GOUGE,

		MAX
	};

	PlayerGouge(const VECTOR& playerPos, const VECTOR& playerAngle);
	~PlayerGouge();

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override;

	int GetState(void)const { return (int)state_; }

	void On(void);
	void Off(void);
	bool Gouge(void);

private:
	STATE state_;

	const float STATE_RADIUS[(int)STATE::MAX] = { 10.0f,150.0f };

	float xAngle_;

	bool searchHit_;
	bool gougeHit_;

	const VECTOR LOCAL_POS = { 0.0f,150.0f,0.0f };
	const VECTOR FOOT_POS = { 0.0f,-90.0f,0.0f };
	const VECTOR& playerPos;
	const VECTOR& playerAngle;
};