#include"PlayerHpBarManager.h"

#include"../../../../../Utility/Utility.h"

PlayerHpBarManager::PlayerHpBarManager(const unsigned char& HP, const unsigned char HP_MAX):
	HP(HP),
	prevHP(this->HP),
	HP_MAX(HP_MAX),

	hpBar(),

	hpBarFrameImageHandle(-1),

	position(),

	aliveHpBarNum(0),
	totalHpBarNum(0),

	hpBarDropIntervalCounter(0)
{
}

PlayerHpBarManager::~PlayerHpBarManager()
{
}

void PlayerHpBarManager::Load(void)
{
	Utility::LoadImg(hpBarFrameImageHandle, "Data/Image/Game/UI/PlayerHpBarFrame.png");

	for (unsigned char i = 0; i < HP_BAR_DIVISIONS_NUM; i++) {
		hpBar[i] = new PlayerHpBar(position, HP_BAR_ONE_DIVISION_SIZE);
	}
}

void PlayerHpBarManager::Init(const Vector2& position)
{
	this->position = position;

	prevHP = HP;

	totalHpBarNum = aliveHpBarNum = HP_BAR_DIVISIONS_NUM;

	hpBarDropIntervalCounter = 0;

	unsigned char num = 0;
	Vector2 hpBarAlivePos = HP_BAR_FIRST_POS;

	for (PlayerHpBar*& h : hpBar) {
		num++;

		h->Init(hpBarAlivePos, GetColor(num, 255, num));

		hpBarAlivePos += ((num % HP_BAR_DIVISION_NUM_Y) == 0) ? HP_BAR_NEXT_POS_UNIQUE : HP_BAR_NEXT_POS_USUALLY;
	}
}

void PlayerHpBarManager::Update(void)
{	
	// HPが変化したかどうか
	if (HP != prevHP) {

		// 変化した数値を保持
		prevHP = HP;

		// 最大HPに対する現在のHPの割合を算出
		const float hpRatio = (float)HP / (float)HP_MAX;

		// 生きているHPバーブロックの数を算出
		const unsigned char newAliveHpBarNum = (unsigned char)(hpRatio * (float)HP_BAR_DIVISIONS_NUM);

		// 死んだHPバーブロックの処理
		for (unsigned char i = newAliveHpBarNum; i < aliveHpBarNum; i++) { hpBar[i]->SetLostIdle(); }

		// 変化した数値を保持
		aliveHpBarNum = newAliveHpBarNum;
	}

	if (totalHpBarNum > aliveHpBarNum) {
		if (++hpBarDropIntervalCounter >= HP_BAR_DROP_INTERVAL) {
			hpBarDropIntervalCounter = 0;
			hpBar[totalHpBarNum - 1]->SetLostDrop();
			totalHpBarNum--;
		}
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
