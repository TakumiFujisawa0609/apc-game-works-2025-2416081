#include"PlayerHpBar.h"

#include<DxLib.h>

PlayerHpBar::PlayerHpBar(const Vector2& position, float HP_BAR_ONE_DIVISION_SIZE) :
	position(position),
	HP_BAR_ONE_DIVISION_SIZE(HP_BAR_ONE_DIVISION_SIZE),

	angle(0.0f),

	state(STATE::NON)
{
}

PlayerHpBar::~PlayerHpBar()
{
}

void PlayerHpBar::Init(const Vector2& localAlivePosition)
{
	localPosition = localAlivePosition;

	stateFuncPtr[(int)STATE::NON] = &PlayerHpBar::Non;
	stateFuncPtr[(int)STATE::ALIVE] = &PlayerHpBar::Alive;
	stateFuncPtr[(int)STATE::LOST_IDLE] = &PlayerHpBar::LostIdle;
	stateFuncPtr[(int)STATE::LOST_DROP] = &PlayerHpBar::LostDrop;

	state = STATE::ALIVE;
}

void PlayerHpBar::Update(void)
{
	(this->*stateFuncPtr[(int)state])();
}

void PlayerHpBar::Draw(void)
{
	if (state == STATE::NON) { return; }

	auto drawPoss = DrawPositionVertexs();

	DrawQuadrangleAA(
		drawPoss.topLeft.x, drawPoss.topLeft.y,
		drawPoss.topRight.x, drawPoss.topRight.y,
		drawPoss.bottomRight.x, drawPoss.bottomRight.y,
		drawPoss.bottomLeft.x, drawPoss.bottomLeft.y,
		(state == STATE::ALIVE) ? 0x00ff00 : 0xff0000,
		true
	);
}

void PlayerHpBar::LostIdle(void)
{
}

void PlayerHpBar::LostDrop(void)
{
}