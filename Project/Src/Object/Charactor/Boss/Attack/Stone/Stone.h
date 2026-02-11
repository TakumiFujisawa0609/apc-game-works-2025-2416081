#pragma once

#include"../../../../ActorBase.h"

class Stone : public ActorBase
{
public:
	Stone(int model);
	~Stone();

	void Load(void)override;

	void On(const Vector3& pos, const Vector3& moveVec);

	virtual void OnCollision(const ColliderBase& collider);

private:
#pragma region 定数定義
	// 半径
	const float RADIUS = 100.0f;
	// スピード
	const float MOVE_SPEED = 30.0f;
	// 生存時間
	const int ALIVE_TIME = 200;
#pragma endregion

	// 移動ベクトル
	Vector3 moveVec;

	// 生存時間計測用カウンター
	int aliveCounter;

	void SubInit(void)override;
	void SubUpdate(void)override;
};
