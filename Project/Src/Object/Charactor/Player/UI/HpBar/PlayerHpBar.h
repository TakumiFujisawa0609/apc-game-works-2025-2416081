#pragma once

#include"../../../../../Common/Vector2.h"

class PlayerHpBar
{
public:
	PlayerHpBar(const Vector2& position, float HP_BAR_ONE_DIVISION_SIZE);
	~PlayerHpBar();

	void Init(const Vector2& localAlivePosition);
	void Update(void);
	void Draw(void);

	// 状態
	enum class STATE
	{
		NON,
		ALIVE,
		LOST_IDLE,
		LOST_DROP,

		MAX
	};

	// 状態取得
	STATE GetState(void) { return state; }

	// 状態設定
	void SetLostIdle(void) { state = STATE::LOST_IDLE; }
	void SetLostDrop(void) { state = STATE::LOST_DROP; }

private:
	// 大きさ
	const float HP_BAR_ONE_DIVISION_SIZE;

	// 座標
	const Vector2& position;
	Vector2 localPosition;
	Vector2 DrawPosition(void) { return position + localPosition; }

	// 描画頂点オフセット
	const Vector2 TOP_LEFT_OFFSET = Vector2(-HP_BAR_ONE_DIVISION_SIZE * 0.5f, -HP_BAR_ONE_DIVISION_SIZE * 0.5f);
	const Vector2 TOP_RIGHT_OFFSET = Vector2(HP_BAR_ONE_DIVISION_SIZE * 0.5f, -HP_BAR_ONE_DIVISION_SIZE * 0.5f);
	const Vector2 BOTTOM_LEFT_OFFSET = Vector2(-HP_BAR_ONE_DIVISION_SIZE * 0.5f, HP_BAR_ONE_DIVISION_SIZE * 0.5f);
	const Vector2 BOTTOM_RIGHT_OFFSET = Vector2(HP_BAR_ONE_DIVISION_SIZE * 0.5f, HP_BAR_ONE_DIVISION_SIZE * 0.5f);

	// 描画頂点座標取得
	struct {
		Vector2 topLeft;
		Vector2 topRight;
		Vector2 bottomLeft;
		Vector2 bottomRight;
	} DrawPositionVertexs(void) {
		Vector2 drawPos = DrawPosition();
		return{
			drawPos + TOP_LEFT_OFFSET.TransMat(angle),
			drawPos + TOP_RIGHT_OFFSET.TransMat(angle),
			drawPos + BOTTOM_LEFT_OFFSET.TransMat(angle),
			drawPos + BOTTOM_RIGHT_OFFSET.TransMat(angle)
		};
	}

	// 角度
	float angle;

	// 状態
	STATE state;

	// 状態別更新処理
	void Non(void) {}
	void Alive(void) {}
	void LostIdle(void);
	void LostDrop(void);

	// 状態関数ポインタ型定義
	using STATEFUNC = void (PlayerHpBar::*)(void);
	// 状態関数ポインタ配列
	STATEFUNC stateFuncPtr[(int)STATE::MAX];

};