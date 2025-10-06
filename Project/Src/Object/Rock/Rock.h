#pragma once

#include<DxLib.h>
#include<vector>

#include"../VoxelBase.h"

class Rock : public VoxelBase
{
public:
	static constexpr VECTOR SIZE = { 236.0f, 293.0f, 325.0f };
	//static constexpr VECTOR CENTER_DIFF = { -24.0f, 136.0f, 10.0f };
	//static constexpr VECTOR CENTER_DIFF = { -160.0f, 0.0f, -100.0f };
	static constexpr VECTOR CENTER_DIFF = { 0.0f, SIZE.y/2, 0.0f };

	Rock();
	~Rock()override {};

	void OnCollision(UnitBase* other)override;

	void SetSpeed(float s) { unit_.para_.speed = s; }
	void InitMove(void) { unit_.pos_ = VAdd(unit_.pos_, VScale({ 1.0f,0.0f,0.0f }, unit_.para_.speed)); }
private:
	void SubLoad(void)override;
	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override;
	void SubRelease(void)override;

};