#include"BossHpBar.h"

#include"../../../../../Utility/Utility.h"

#include"../../../../../Application/Application.h"

BossHpBar::BossHpBar(const Vector2& position, float HP_BAR_ONE_DIVISION_SIZE) :
	position(position),
	HP_BAR_ONE_DIVISION_SIZE(HP_BAR_ONE_DIVISION_SIZE),

	angle(0.0f),

	state(STATE::NON),
	stateFuncPtr(),

	idleTimer(0),
	idleShakeSign(1),

	ALIVE_COLOR(0x00ff00)
{
}

void BossHpBar::Init(const Vector2& localAlivePosition, unsigned int aliveColor)
{
	localPosition = localAlivePosition;

	ALIVE_COLOR = aliveColor;

	angle = 0.0f;

	idleTimer = 0;

	state = STATE::ALIVE;

	stateFuncPtr[(int)STATE::NON] = &BossHpBar::Non;
	stateFuncPtr[(int)STATE::ALIVE] = &BossHpBar::Alive;
	stateFuncPtr[(int)STATE::LOST_IDLE] = &BossHpBar::LostIdle;
	stateFuncPtr[(int)STATE::LOST_DROP] = &BossHpBar::LostDrop;
}

void BossHpBar::Update(void)
{
	(this->*stateFuncPtr[(int)state])();
}

void BossHpBar::Draw(void)
{
	if (state == STATE::NON) { return; }

	auto drawPoss = DrawPositionVertexs();

	DrawQuadrangleAA(
		drawPoss.topLeft.x, drawPoss.topLeft.y,
		drawPoss.topRight.x, drawPoss.topRight.y,
		drawPoss.bottomRight.x, drawPoss.bottomRight.y,
		drawPoss.bottomLeft.x, drawPoss.bottomLeft.y,
		(state == STATE::ALIVE) ? ALIVE_COLOR : 0xff0000,
		true
	);
}


void BossHpBar::SetLostIdle(void)
{
	state = STATE::LOST_IDLE;

	localPosition += (float)idleShakeSign * 0.5f;
	idleShakeSign *= -1;
}

void BossHpBar::LostIdle(void)
{
	if (idleTimer % 3 == 0) {
		localPosition += (float)idleShakeSign;
		idleShakeSign *= -1;
	}

	idleTimer++;
}

void BossHpBar::SetLostDrop(void)
{
	state = STATE::LOST_DROP;

	dropAccel = Vector2(-(float)(GetRand(3) + 2), -(float)(GetRand(3) + 2));
}
void BossHpBar::LostDrop(void)
{
	// ‰ñ“]‚³‚¹‚é
	angle += Utility::Deg2RadF(5.0f);

	// —Ž‰º
	localPosition += dropAccel;
	dropAccel.y += DROP_GRAVITY;

	// ‰æ–ÊŠO‚Éo‚½‚çÁ‚·
	if (DrawPosition().y - HP_BAR_ONE_DIVISION_SIZE > App::SCREEN_SIZE_Y) { state = STATE::NON; }
}