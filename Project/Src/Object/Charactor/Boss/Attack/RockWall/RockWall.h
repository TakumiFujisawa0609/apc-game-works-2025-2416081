#pragma once

#include<DxLib.h>
#include<vector>

#include"../../../../VoxelBase.h"

class RockWall : public VoxelBase
{
public:
	RockWall(int modelId, int textureId);
	~RockWall()override {};

	void OnCollision(const ColliderBase& collider)override;

	// ó‘Ô‚Ì—ñ‹“Œ^’è‹`
	enum class STATE
	{
		NON,
		MOVE,
		BE,

		MAX
	};

	void On(const Vector3& pos);

private:

#pragma region ’è”’è‹`
	const Vector3 SIZE = { 250.0f, 293.0f, 325.0f };
	const Vector3 CENTER_DIFF = { -10.0f, SIZE.y / 2 - 10.0f, 0.0f };
#pragma endregion

	void SubLoad(void)override;
	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubDraw(void)override {}
	void SubAlphaDraw(void)override {}
	void SubRelease(void)override {}

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
	// Move``````
	const float MOVE_SPEED = 20.0f;
	unsigned char stageCollisionFlg_;
	//`````````

	// Be```````
	
	//`````````
#pragma endregion
};