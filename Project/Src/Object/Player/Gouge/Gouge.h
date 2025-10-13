#pragma once

#include"../../UnitBase.h"

class Gouge :public UnitBase
{
public:
	Gouge(const VECTOR& playerPos, const VECTOR& playerAngle);
	~Gouge();

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override;

private:

	float xAngle_;

	const VECTOR LOCAL_POS = { 0.0f,90.0f,0.0f };
	const VECTOR FOOT_POS = { 0.0f,-90.0f,0.0f };
	const VECTOR& playerPos;
	const VECTOR& playerAngle;
};