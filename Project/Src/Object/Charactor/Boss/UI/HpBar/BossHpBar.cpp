#include"BossHpBar.h"

#include<string>

#include"../../../../../Utility/Utility.h"

BossHpBar::BossHpBar(const unsigned short& HP, const unsigned short HP_MAX, const unsigned char& LIFE, unsigned char number) :
	HP(HP),
	prevHP(this->HP),
	HP_MAX(HP_MAX),

	LIFE(LIFE),

	NUMBER(number),

	hpBar(),

	position(),

	aliveHpBarNum(0),
	totalHpBarNum(0),

	hpBarDropIntervalCounter(0)
{
}

void BossHpBar::Load(void)
{
	for (unsigned short i = 0; i < HP_BAR_DIVISIONS_NUM; i++) {
		hpBar[i] = new BossHpBlock(position, HP_BAR_ONE_DIVISION_SIZE);
	}
}

void BossHpBar::Init(const Vector2& position, unsigned int color)
{
	this->position = position;

	prevHP = HP;

	totalHpBarNum = aliveHpBarNum = HP_BAR_DIVISIONS_NUM;

	hpBarDropIntervalCounter = 0;

	unsigned short num = 0;
	Vector2 hpBarAlivePos = HP_BAR_FIRST_POS;

	for (BossHpBlock*& h : hpBar) {
		num++;

		unsigned char colorAround = num / 4;

		h->Init(hpBarAlivePos, num, HP_BAR_DIVISIONS_NUM);
		h->SetDefaultColor(color);

		hpBarAlivePos += ((num % HP_BAR_DIVISION_NUM_Y) == 0) ? HP_BAR_NEXT_POS_UNIQUE : HP_BAR_NEXT_POS_USUALLY;
	}
}

void BossHpBar::Update(void)
{
	// 現在のライフに合わせてHPバーの状態を更新する
	if (LIFE - 1 == NUMBER) {
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

			// 復活したHPバーブロックの処理
			for (unsigned short i = aliveHpBarNum; i < newAliveHpBarNum; i++) { hpBar[i]->Revival(i + 1); }

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

		if (totalHpBarNum < aliveHpBarNum) { totalHpBarNum = aliveHpBarNum; }
	}

	// HPバーの状態を更新
	for (BossHpBlock*& h : hpBar) { h->Update(); }
}

void BossHpBar::Draw(void)
{
	// HPバーの描画
	for (BossHpBlock*& h : hpBar) { h->Draw(); }
}

void BossHpBar::Release(void)
{
	// HPバーの解放
	for (BossHpBlock*& h : hpBar) {
		if (!h) { continue; }
		delete h;
	}
}
