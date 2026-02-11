#pragma once

#include"../../../ActorBase.h"

class PlayerGouge : public ActorBase
{
public:
	enum class STATE
	{
		NON = -1,

		SEARCH,
		GOUGE,

		MAX
	};

	PlayerGouge(const Vector3& playerPos, const Vector3& playerAngle);
	~PlayerGouge()override {}

	void Load(void)override;

	void OnCollision(const ColliderBase& collider)override;

	int GetState(void)const { return (int)state; }

	void On(void);
	void Off(void);
	bool Gouge(void);

private:

	void SubInit(void)override;
	void SubUpdate(void)override;

	// 状態
	STATE state;

	// 状態別半径
	const float STATE_RADIUS[(int)STATE::MAX] = { 30.0f,120.0f };

	// 探索中の角度
	float xAngle;

	// 探索(探索中に破壊可能オブジェクトにぶつかったか)の判別フラグ
	bool searchHit;
	// 破壊は完了したかの判別フラグ
	bool gougeHit_;

	// 探索位置相対座標
	const Vector3 FOOT_POS = { 0.0f,-90.0f,0.0f };
	const Vector3 LOCAL_POS = { 0.0f,150.0f,0.0f };

	// プレイヤー座標(参照用)
	const Vector3& playerPos;
	// プレイヤー角度(参照用)
	const Vector3& playerAngle;
};