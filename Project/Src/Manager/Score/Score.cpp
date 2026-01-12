#include"Score.h"

Score* Score::ins = nullptr;

Score::Score() :
	totalScore(),
	addScore(),

	attackCombo(0),
	attackComboTime(0),
	attackComboInterval(0),

	breakCombo(0),
	breakComboTime(0),
	breakComboInterval(0)
{
}

void Score::Update(void)
{
#pragma region コンボ更新
	// アタックコンボの加算インターバルを更新
	if (++attackComboInterval > ATTACK_COMBO_INTERVAL) { attackComboInterval = ATTACK_COMBO_INTERVAL; }

	// アタックコンボの継続時間を更新
	if (++attackComboTime > ATTACK_COMBO_TIME) {
		attackComboTime = 0;
		attackCombo = 0;
	}

	// ブレイクコンボの加算インターバルを更新
	if (++breakComboInterval > BREAK_COMBO_INTERVAL) { breakComboInterval = BREAK_COMBO_INTERVAL; }

	// ブレイクコンボの継続時間を更新
	if (++breakComboTime > BREAK_COMBO_TIME) {
		breakComboTime = 0;
		breakCombo = 0;
	}
#pragma endregion
}

void Score::Reset(void)
{
	totalScore = 0;
	addScore.clear();

	attackCombo = 0;
	attackComboTime = 0;
	attackComboInterval = 0;

	breakCombo = 0;
	breakComboTime = 0;
	breakComboInterval = 0;
}

#pragma region スコア加算

bool Score::ScoreAddOther(int add)
{
	if (add <= 0) { return false; }
	addScore.emplace_back((int)((float)add * Rate()));
	return true;
}

void Score::ScoreAddAttack(int add)
{
	if (!ScoreAddOther(add)) { return; }
	attackComboTime = 0;
	if (attackComboInterval >= ATTACK_COMBO_INTERVAL) {
		attackComboInterval = 0;
		if (++attackCombo > ATTACK_COMBO_MAX) { attackCombo = ATTACK_COMBO_MAX; }
	}
}

void Score::ScoreAddBreak(int add)
{
	if (!ScoreAddOther(add)) { return; }
	breakComboTime = 0;
	if (breakComboInterval >= BREAK_COMBO_INTERVAL) {
		breakComboInterval = 0;
		if (++breakCombo > BREAK_COMBO_MAX) { breakCombo = BREAK_COMBO_MAX; }
	}
}

int Score::AddScoreApplyAndGet(void)
{
	// 加算予定スコアがない場合は０を返して終了
	if (addScore.empty()) { return 0; }

	// 加算予定スコアのリストの先頭の数値を 取得/破棄
	int addAmount = addScore.front();
	addScore.pop_front();

	// 現在のスコアに加算
	totalScore += addAmount;

	// 加算量を返す
	return addAmount;
}

#pragma endregion