#include"PlayerHpBar.h"

#include"../../../../../Utility/Utility.h"

#include"../../../../../Application/Application.h"

PlayerHpBar::PlayerHpBar(const Vector2& position, float HP_BAR_ONE_DIVISION_SIZE) :
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

PlayerHpBar::~PlayerHpBar()
{
}

void PlayerHpBar::Init(const Vector2& localAlivePosition, unsigned short num, unsigned char HP_BAR_DIVISIONS_NUM)
{
	localPosition = localAlivePosition;

	angle = 0.0f;

	idleTimer = num;

	this->HP_BAR_DIVISION_NUM = HP_BAR_DIVISIONS_NUM;

	state = STATE::ALIVE;

	stateFuncPtr[(int)STATE::NON] = &PlayerHpBar::Non;
	stateFuncPtr[(int)STATE::ALIVE] = &PlayerHpBar::Alive;
	stateFuncPtr[(int)STATE::LOST_IDLE] = &PlayerHpBar::LostIdle;
	stateFuncPtr[(int)STATE::LOST_DROP] = &PlayerHpBar::LostDrop;
}

void PlayerHpBar::Update(void)
{
	(this->*stateFuncPtr[(int)state])();
}

void PlayerHpBar::Draw(void)
{
	if (state == STATE::NON) { return; }

	auto drawPoss = DrawPositionVertexs();

	unsigned int aliveColor =
		GetColor(
			std::clamp((unsigned int)ALIVE_DEFAULT_R + idleTimer, 0u, 255u),
			std::clamp((unsigned int)ALIVE_DEFAULT_G + idleTimer, 0u, 255u),
			std::clamp((unsigned int)ALIVE_DEFAULT_B + idleTimer, 0u, 255u)
		);

	DrawQuadrangleAA(
		drawPoss.topLeft.x, drawPoss.topLeft.y,
		drawPoss.topRight.x, drawPoss.topRight.y,
		drawPoss.bottomRight.x, drawPoss.bottomRight.y,
		drawPoss.bottomLeft.x, drawPoss.bottomLeft.y,
		(state == STATE::ALIVE) ? aliveColor : 0xff0000,
		true
	);
}

void PlayerHpBar::Alive(void)
{
	idleTimer += idleShakeSign;
	if (idleTimer > HP_BAR_DIVISION_NUM || idleTimer <= 0) { idleShakeSign *= -1; }
}

void PlayerHpBar::SetLostIdle(void)
{
	state = STATE::LOST_IDLE;

	idleTimer = 0;

	localPosition += (float)idleShakeSign * 0.5f;
	idleShakeSign *= -1;
}

void PlayerHpBar::LostIdle(void)
{
	if (idleTimer % 3 == 0) {
		localPosition += (float)idleShakeSign;
		idleShakeSign *= -1;
	}

	idleTimer++;
}

void PlayerHpBar::SetLostDrop(void)
{
	state = STATE::LOST_DROP;

	dropAccel = Vector2((float)(GetRand(3) + 2), -(float)(GetRand(3) + 2));
}
void PlayerHpBar::LostDrop(void)
{
	// ‰ñ“]‚³‚¹‚é
	angle += Utility::Deg2RadF(5.0f);

	// —Ž‰º
	localPosition += dropAccel;
	dropAccel.y += DROP_GRAVITY;

	// ‰æ–ÊŠO‚Éo‚½‚çÁ‚·
	if (DrawPosition().y - HP_BAR_ONE_DIVISION_SIZE > App::SCREEN_SIZE_Y) { state = STATE::NON; }
}