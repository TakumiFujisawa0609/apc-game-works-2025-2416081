#include"PlayerHpBlock.h"

#include"../../../../../Utility/Utility.h"

#include"../../../../../Application/Application.h"

PlayerHpBlock::PlayerHpBlock(const Vector2& position, float HP_BAR_ONE_DIVISION_SIZE) :
	position(position),
	HP_BAR_ONE_DIVISION_SIZE(HP_BAR_ONE_DIVISION_SIZE),

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

PlayerHpBlock::~PlayerHpBlock()
{
}

void PlayerHpBlock::Init(const Vector2& localAlivePosition, unsigned short num, unsigned char HP_BAR_DIVISIONS_NUM)
{
	localPosition = localAlivePosition;

	angle = 0.0f;

	idleTimer = num;

	this->HP_BAR_DIVISION_NUM = HP_BAR_DIVISIONS_NUM;

	state = STATE::ALIVE;

	stateFuncPtr[(int)STATE::NON] = &PlayerHpBlock::Non;
	stateFuncPtr[(int)STATE::ALIVE] = &PlayerHpBlock::Alive;
	stateFuncPtr[(int)STATE::LOST_IDLE] = &PlayerHpBlock::LostIdle;
	stateFuncPtr[(int)STATE::LOST_DROP] = &PlayerHpBlock::LostDrop;
}

void PlayerHpBlock::Update(void)
{
	(this->*stateFuncPtr[(int)state])();
}

void PlayerHpBlock::Draw(void)
{
	if (state == STATE::NON) { return; }

	auto drawPoss = DrawPositionVertexs();

	unsigned int aliveColor =
		GetColor(
			std::clamp((unsigned int)ALIVE_DEFAULT_R + idleTimer, 0u, 255u),
			std::clamp((unsigned int)ALIVE_DEFAULT_G + idleTimer, 0u, 255u),
			std::clamp((unsigned int)ALIVE_DEFAULT_B + idleTimer, 0u, 255u)
		);

	DrawQuadrangle(
		(int)drawPoss.topLeft.x, (int)drawPoss.topLeft.y,
		(int)drawPoss.topRight.x, (int)drawPoss.topRight.y,
		(int)drawPoss.bottomRight.x, (int)drawPoss.bottomRight.y,
		(int)drawPoss.bottomLeft.x, (int)drawPoss.bottomLeft.y,
		(state == STATE::ALIVE) ? aliveColor : LOST_COLOR,
		true
	);
}

void PlayerHpBlock::Alive(void)
{
	idleTimer += idleShakeSign;
	if (idleTimer > HP_BAR_DIVISION_NUM || idleTimer <= 0) { idleShakeSign *= -1; }
}

void PlayerHpBlock::SetLostIdle(void)
{
	state = STATE::LOST_IDLE;

	idleTimer = 0;

	localPosition += (float)idleShakeSign * LOST_SHAKE_SIZE;
	idleShakeSign *= -1;
}

void PlayerHpBlock::LostIdle(void)
{
	if (idleTimer % LOST_SHAKE_CYCLE == 0) {
		localPosition += (float)idleShakeSign;
		idleShakeSign *= -1;
	}

	idleTimer++;
}

void PlayerHpBlock::SetLostDrop(void)
{
	state = STATE::LOST_DROP;

	dropAccel = DropInitVelocity();
}
void PlayerHpBlock::LostDrop(void)
{
	// ‰ñ“]‚³‚¹‚é
	angle += Deg2Rad(DROP_ROTATE_SPEED);

	// —Ž‰º
	localPosition += dropAccel;
	dropAccel.y += DROP_GRAVITY;

	// ‰æ–ÊŠO‚Éo‚½‚çÁ‚·
	if (DrawPosition().y - HP_BAR_ONE_DIVISION_SIZE > App::SCREEN_SIZE_Y) { state = STATE::NON; }
}