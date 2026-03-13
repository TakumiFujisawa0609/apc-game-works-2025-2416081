#include"BossHpBlock.h"

#include"../../../../../Utility/Utility.h"

#include"../../../../../Application/Application.h"

BossHpBlock::BossHpBlock(const Vector2& position, float HP_BAR_ONE_DIVISION_SIZE) :
	position(position),
	HP_BAR_ONE_DIVISION_SIZE(HP_BAR_ONE_DIVISION_SIZE),

	ALIVE_LOCAL_POSITION(),
	localPosition(),

	angle(0.0f),

	state(STATE::NON),
	stateFuncPtr(),

	idleTimer(0),
	idleShakeSign(1),

	ALIVE_DEFAULT_R(0),
	ALIVE_DEFAULT_G(0),
	ALIVE_DEFAULT_B(0),

	HP_BAR_DIVISION_NUM(0)
{
}

void BossHpBlock::Init(const Vector2& localAlivePosition, unsigned short num, unsigned short HP_BAR_DIVISIONS_NUM)
{
	localPosition = ALIVE_LOCAL_POSITION = localAlivePosition;

	angle = 0.0f;

	idleTimer = num;

	this->HP_BAR_DIVISION_NUM = HP_BAR_DIVISIONS_NUM;

	state = STATE::ALIVE;

	stateFuncPtr[(int)STATE::NON] = &BossHpBlock::Non;
	stateFuncPtr[(int)STATE::ALIVE] = &BossHpBlock::Alive;
	stateFuncPtr[(int)STATE::LOST_IDLE] = &BossHpBlock::LostIdle;
	stateFuncPtr[(int)STATE::LOST_DROP] = &BossHpBlock::LostDrop;
}

void BossHpBlock::Update(void)
{
	(this->*stateFuncPtr[(int)state])();
}

void BossHpBlock::Draw(void)
{
	if (state == STATE::NON) { return; }

	BoxVertexs drawPoss = DrawPositionVertexs();

	unsigned int aliveColor =
		GetColor(
			std::clamp((unsigned int)ALIVE_DEFAULT_R + (idleTimer / 2), 0u, 255u),
			std::clamp((unsigned int)ALIVE_DEFAULT_G + (idleTimer / 2), 0u, 255u),
			std::clamp((unsigned int)ALIVE_DEFAULT_B + (idleTimer / 2), 0u, 255u)
		);

	DrawQuadrangle(
		(int)drawPoss.topLeft.x, (int)drawPoss.topLeft.y,
		(int)drawPoss.topRight.x, (int)drawPoss.topRight.y,
		(int)drawPoss.bottomRight.x, (int)drawPoss.bottomRight.y,
		(int)drawPoss.bottomLeft.x, (int)drawPoss.bottomLeft.y,
		(state == STATE::ALIVE) ? aliveColor : 0xff0000,
		true
	);
}

void BossHpBlock::Alive(void)
{
	idleTimer += idleShakeSign;
	if (idleTimer > HP_BAR_DIVISION_NUM || idleTimer <= 0) { idleShakeSign *= -1; }
}

void BossHpBlock::SetLostIdle(void)
{
	state = STATE::LOST_IDLE;

	localPosition += (float)idleShakeSign * 0.5f;
	idleShakeSign *= -1;
}

void BossHpBlock::LostIdle(void)
{
	if (idleTimer % 3 == 0) {
		localPosition += (float)idleShakeSign;
		idleShakeSign *= -1;
	}

	idleTimer++;
}

void BossHpBlock::SetLostDrop(void)
{
	state = STATE::LOST_DROP;

	dropAccel = Vector2(-(float)(GetRand(3) + 2), -(float)(GetRand(3) + 2));
}

void BossHpBlock::Revival(unsigned short num)
{
	localPosition = ALIVE_LOCAL_POSITION;

	angle = 0.0f;

	idleTimer = num;

	idleShakeSign = 1;

	state = STATE::ALIVE;
}

void BossHpBlock::LostDrop(void)
{
	// ‰ñ“]‚³‚¹‚é
	angle += Deg2Rad(5.0f);

	// —Ž‰º
	localPosition += dropAccel;
	dropAccel.y += DROP_GRAVITY;

	// ‰æ–ÊŠO‚Éo‚½‚çÁ‚·
	if (DrawPosition().y - HP_BAR_ONE_DIVISION_SIZE > App::SCREEN_SIZE_Y) { state = STATE::NON; }
}