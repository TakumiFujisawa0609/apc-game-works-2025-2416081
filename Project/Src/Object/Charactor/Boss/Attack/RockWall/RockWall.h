#pragma once

#include<DxLib.h>
#include<vector>

#include"../../../VoxelBase.h"

class RockWall : public VoxelBase
{
public:
	static constexpr VECTOR SIZE = { 250.0f, 293.0f, 325.0f };
	static constexpr VECTOR CENTER_DIFF = { -10.0f, SIZE.y / 2 - 10.0f, 0.0f };

	RockWall(int textureId);
	~RockWall()override {};

	void OnCollision(UnitBase* other)override;

	int GetState(void)const override { return (int)state_; }

	enum class STATE
	{
		NON,
		MOVE,
		BE,

		MAX
	};

	void On(const VECTOR& pos);

private:
	void SubLoad(void)override;
	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override;
	void SubRelease(void)override;

#pragma region ó‘ÔŠÇ—
	STATE state_;
	// ŠÖ”ƒ|ƒCƒ“ƒ^”z—ñ
	using STATEFUNC = void (RockWall::*)(void);
	STATEFUNC stateFuncPtr[(int)STATE::MAX];

	// ó‘Ô•ÊŠÖ”````
	void Non(void) {}
	void Move(void);
	void Be(void);
	//`````````
#pragma endregion

#pragma region ó‘Ô•ÊŠÖ”‚Ì’†g
	// Create`````
	const int CREATE_IDLE_TIME = 180;	//¶¬‚©‚ç“®‚«o‚·‚Ü‚Å‚ÌŠÔ
	int createIdleCounter_;
	//`````````

	// Move``````
	const float MOVE_SPEED = 20.0f;
	
	//`````````

	// Be```````
	
	//`````````
#pragma endregion
};