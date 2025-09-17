#pragma once

#include"../../UnitBase.h"

class PlayerPunch : public UnitBase
{
public:
	static constexpr float RADIUS = 150.0f;
	static constexpr VECTOR LOCAL_POS = { 0.0f,0.0f,-100.0f };


	PlayerPunch(const VECTOR& playerPos, const VECTOR& playerAngle);
	~PlayerPunch()override {};

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override;

	void On(void) { unit_.isAlive_ = true; }
	void Off(void) { unit_.isAlive_ = false; }

private:
	const VECTOR& playerPos;
	const VECTOR& playerAngle;

};

