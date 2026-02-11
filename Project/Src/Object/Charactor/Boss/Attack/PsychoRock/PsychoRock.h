#pragma once

#include"../../../../ActorBase.h"

class PsychoRock : public ActorBase
{
public:
	PsychoRock(int model, const Vector3& playerPos);
	~PsychoRock();

	void Load(void)override;

	enum class STATE {
		NON,
		RISE_PREPARA,
		RISE,
		SHOT,

		MAX,
	};
	STATE GetState(void)const { return state_; }

	void Set(float x, float z);
	void On(void);

private:
	void SubInit(void)override;
	void SubUpdate(void)override;
	void SubAlphaDraw(void)override;

	// 状態
	STATE state_;

	// 関数ポインタ定義用
	using STATEFUNC = void (PsychoRock::*)(void);

	void Non(void) {};

	// 状態別更新処理を格納する関数ポインタ配列
	STATEFUNC stateUpdateFuncPtr[(int)STATE::MAX];
	void RisePreparaUpdate(void);
	void RiseUpdate(void);
	void ShotUpdate(void);

	// 状態別描画処理を格納する関数ポインタ配列
	STATEFUNC stateDrawFuncPtr[(int)STATE::MAX];
	void RisePreparaDraw(void);
	void RiseDraw(void);
	void ShotDraw(void);

	// モデルスケール
	const float SCALE = 1.5f;
	// 半径
	const float RADIUS = 65.0f * SCALE;

	// 待機時間
	const int PREPARA_TIME = 500;
	// 待機時間計測用カウンター
	int preparaTimer_;

	// 岩が持ち上がるときのスピード
	const float RISE_SPEED = 10.0f;

	// 放たれたときのスピード
	const float SHOT_SPEED = 30.0f;

	// 移動ベクトル
	Vector3 moveVec_;
	// ターゲット座標
	Vector3 targetPos_;

	// プレイヤー座標(参照用)
	const Vector3& playerPos;
};