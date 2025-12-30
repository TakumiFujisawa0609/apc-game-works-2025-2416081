#include"PlayerHpBarMamager.h"

#include"../../../../../Utility/Utility.h"

PlayerHpBarManager::PlayerHpBarManager(const unsigned char& HP, const unsigned char HP_MAX):
	HP(HP),
	HP_MAX(HP_MAX),

	hpBar(),

	hpBarFrameImageHandle(-1),

	position()
{
}

PlayerHpBarManager::~PlayerHpBarManager()
{
}

void PlayerHpBarManager::Load(void)
{
	Utility::LoadImg(hpBarFrameImageHandle, "Data/Image/UI/Player/PlayerHpBarFrame.png");

	for (unsigned char i = 0; i < HP_BAR_DIVISIONS_NUM; i++) {
		hpBar[i] = new PlayerHpBar(position, HP_BAR_ONE_DIVISION_SIZE);
	}
}

void PlayerHpBarManager::Init(void)
{
	prevHP = HP;

	totalHpBarNum = aliveHpBarNum = HP_BAR_DIVISIONS_NUM;

	for (PlayerHpBar*& h : hpBar) {
		h->Init(Vector2(0.0f, 0.0f));
	}
}

void PlayerHpBarManager::Update(void)
{	
	// HP‚ª•Ï‰»‚µ‚½‚©‚Ç‚¤‚©
	if (HP != prevHP) {

		// •Ï‰»‚µ‚½”’l‚ð•ÛŽ
		prevHP = HP;



	}

	if (totalHpBarNum != aliveHpBarNum) {

	}

	for (PlayerHpBar*& h : hpBar) { h->Update(); }
}

void PlayerHpBarManager::Draw(void)
{
	DrawGraphF(position.x, position.y, hpBarFrameImageHandle, true);

	for (PlayerHpBar*& h : hpBar) { h->Draw(); }
}

void PlayerHpBarManager::Release(void)
{
	for (PlayerHpBar*& h : hpBar) {
		if (!h) { continue; }
		delete h;
	}
	DeleteGraph(hpBarFrameImageHandle);
}
