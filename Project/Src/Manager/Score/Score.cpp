#include"Score.h"

Score* Score::ins = nullptr;

Score::Score() :
	baseScore(),
	addScore(),

	combo(0),
	comboTime(0),
	comboInterval(0),
	bestRecordCombo(0)
{
}

void Score::Update(void)
{
#pragma region コンボ更新
	// コンボの加算インターバルを更新
	if (++comboInterval > COMBO_INTERVAL) { comboInterval = COMBO_INTERVAL; }

	// コンボの継続時間を更新
	if (++comboTime > COMBO_TIME) {
		comboTime = 0;
		combo = 0;
	}
#pragma endregion
}

void Score::Reset(void)
{
	baseScore = 0;
	addScore.clear();

	combo = 0;
	comboTime = 0;
	comboInterval = 0;
	bestRecordCombo = 0;
}

#pragma region スコア加算

bool Score::ScoreAddOther(int add)
{
	if (add <= 0) { return false; }
	addScore.emplace_back((int)((float)add * Rate()));
	return true;
}

void Score::ScoreAddCombo(int add)
{
	if (!ScoreAddOther(add)) { return; }
	comboTime = 0;
	if (comboInterval >= COMBO_INTERVAL) {
		comboInterval = 0;
		if (++combo > COMBO_MAX) { combo = COMBO_MAX; }
	}
	if (bestRecordCombo < combo) { bestRecordCombo = combo; }
}

void Score::ScoreAddBonus(int add)
{
	if (add <= 0) { return; }
	addScore.emplace_back(add);
}

int Score::AddBaseScoreApplyAndGet(void)
{
	// 加算予定スコアがない場合は０を返して終了
	if (addScore.empty()) { return 0; }

	// 加算予定スコアのリストの先頭の数値を 取得/破棄
	int addAmount = addScore.front();
	addScore.pop_front();

	// ベーススコアにスコアに加算
	baseScore += addAmount;

	// 加算量を返す
	return addAmount;
}

int Score::AddBonusScoreApplyAndGet(void)
{
	// 加算予定スコアがない場合は０を返して終了
	if (addScore.empty()) { return 0; }

	// 加算予定スコアのリストの先頭の数値を 取得/破棄
	int addAmount = addScore.front();
	addScore.pop_front();

	// ボーナススコアに加算
	bonusScore += addAmount;

	// 加算量を返す
	return addAmount;
}

#pragma endregion