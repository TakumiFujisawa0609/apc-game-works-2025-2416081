#include"BossHpBarManager.h"

#include"../../../../../Utility/Utility.h"

BossHpBarManager::BossHpBarManager(const unsigned short& HP, const unsigned short HP_MAX, unsigned char number) :
	HP(HP),
	prevHP(this->HP),
	HP_MAX(HP_MAX),

	NUMBER(number),

	hpBar(),

	hpBarFrameImageHandle(-1),

	position(),

	aliveHpBarNum(0),
	totalHpBarNum(0),

	hpBarDropIntervalCounter(0)
{
}

void BossHpBarManager::Load(void)
{
	if (NUMBER == 0) { Utility::LoadImg(hpBarFrameImageHandle, "Data/Image/Game/UI/BossHpBarFrame.png"); }

	for (unsigned short i = 0; i < HP_BAR_DIVISIONS_NUM; i++) {
		hpBar[i] = new BossHpBar(position, HP_BAR_ONE_DIVISION_SIZE);
	}
}

void BossHpBarManager::Init(const Vector2& position, unsigned int color)
{
	this->position = position;

	prevHP = HP;

	totalHpBarNum = aliveHpBarNum = HP_BAR_DIVISIONS_NUM;

	hpBarDropIntervalCounter = 0;

	unsigned short num = 0;
	Vector2 hpBarAlivePos = HP_BAR_FIRST_POS;

	for (BossHpBar*& h : hpBar) {
		num++;

		h->Init(hpBarAlivePos, color);

		hpBarAlivePos += ((num % HP_BAR_DIVISION_NUM_Y) == 0) ? HP_BAR_NEXT_POS_UNIQUE : HP_BAR_NEXT_POS_USUALLY;
	}
}

void BossHpBarManager::Update(void)
{
	// HPが変化したかどうか
	if (HP != prevHP) {

		// 変化した数値を保持
		prevHP = HP;

		// 最大HPに対する現在のHPの割合を算出
		const float hpRatio = (float)HP / (float)HP_MAX;

		// 生きているHPバーブロックの数を算出
		const unsigned short newAliveHpBarNum = (unsigned short)(hpRatio * (float)HP_BAR_DIVISIONS_NUM);

		// 死んだHPバーブロックの処理
		for (unsigned short i = newAliveHpBarNum; i < aliveHpBarNum; i++) { hpBar[i]->SetLostIdle(); }

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

	for (BossHpBar*& h : hpBar) { h->Update(); }
}

void BossHpBarManager::Draw(void)
{
	if (NUMBER == 0) { DrawGraphF(position.x, position.y, hpBarFrameImageHandle, true); }

	for (BossHpBar*& h : hpBar) { h->Draw(); }
}

void BossHpBarManager::Release(void)
{
	for (BossHpBar*& h : hpBar) {
		if (!h) { continue; }
		delete h;
	}
	if (NUMBER == 0) { DeleteGraph(hpBarFrameImageHandle); }
}
