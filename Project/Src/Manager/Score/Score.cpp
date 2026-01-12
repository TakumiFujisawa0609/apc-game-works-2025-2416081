#include"Score.h"

Score* Score::ins = nullptr;

Score::Score() :
	totalScore(),
	addScore(),

	damageCombo(0),
	damageComboTime(0),

	breakCombo(0),
	breakComboTime(0)
{
}

void Score::Update(void)
{
#pragma region コンボ更新
	if (++damageComboTime > DAMAGE_COMBO_TIME) {
		damageComboTime = 0;
		damageCombo = 0;
	}
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

	damageCombo = 0;
	damageComboTime = 0;

	breakCombo = 0;
	breakComboTime = 0;
}