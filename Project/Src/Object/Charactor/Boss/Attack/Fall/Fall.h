#pragma once
#include"../../../../ActorBase.h"

class Fall : public ActorBase
{
public:
	Fall(int model);
	~Fall();

	void Load(void)override;

	void OnCollision(const ColliderBase& collider)override;

	enum class STATE
	{
		IDLE,
		FALL,

		MAX
	};

	void Set(const Vector3& pos);
	void On(void) { state = STATE::FALL; }

	STATE GetState(void)const { return state; }

private:

	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubAlphaDraw(void)override;

	// 状態
	STATE state;

	// モデルのスケール
	const float SCALE = 2.0f;

	// 半径
	const float RADIUS = 65.0f * SCALE;

	// 生存時間
	const short IDLE_TIME = 1000;
	// 生存時間計測用カウンター
	short idleCounter;

	// ターゲット座標の高さ
	float groundHeight;
};
