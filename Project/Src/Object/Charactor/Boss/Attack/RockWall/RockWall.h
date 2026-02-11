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
	// ‘å‘Ì‘S‘Ì‚ğ‚¨‚¨‚¦‚éƒTƒCƒY
	const Vector3 ROUGH_SIZE = { 224.68f,278.946f,309.564f };
	// ƒOƒŠƒbƒh’†SÀ•W‚Ö‚Ì·•ª
	const Vector3 GRID_CENTER_DIFF = { -25.382f,135.837f,9.876f };

	// ‚¨‚¨‚Ü‚©‚È”¼Œa
	const float ROUGH_RADIUS = ROUGH_SIZE.Length() / 3.0f;
#pragma endregion

	void SubLoad(void)override;
	void SubInit(void)override;
	void SubUpdate(void)override;

#pragma region ó‘ÔŠÇ—
	// ó‘Ô
	STATE state_;

	// ŠÖ”ƒ|ƒCƒ“ƒ^’è‹`—p
	using STATEFUNC = void (RockWall::*)(void);
	// ó‘Ô•ÊŠÖ”ƒ|ƒCƒ“ƒ^”z—ñ
	STATEFUNC stateFuncPtr[(int)STATE::MAX];

	// ó‘Ô•ÊŠÖ”````
	void Non(void) {}
	void Move(void);
	void Be(void);
	//`````````
#pragma endregion

#pragma region ó‘Ô•ÊŠÖ”‚Ì’†g
	// Move``````

	// ƒXƒs[ƒh
	const float MOVE_SPEED = 20.0f;

	// ƒXƒe[ƒW‚Æ“–‚½‚Á‚Ä‚¢‚é‚©‚ğ”»•Ê‚·‚éƒtƒ‰ƒO
	bool stageCollisionFlg;

	//`````````

	// Be```````
	
	//`````````
#pragma endregion
};